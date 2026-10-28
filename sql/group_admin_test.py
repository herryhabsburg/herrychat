# -*- coding: utf-8 -*-
# 群管理升级联调测试：设置管理员/群昵称/备注/群头像/退出群聊
import socket, struct, json, time, sys

def http_login(email, pwd_cipher):
    body = json.dumps({'email': email, 'passwd': pwd_cipher}).encode()
    s = socket.create_connection(('127.0.0.1', 8080), timeout=5)
    req = (b'POST /user_login HTTP/1.1\r\nHost: 127.0.0.1\r\nContent-Type: application/json\r\nContent-Length: ' +
           str(len(body)).encode() + b'\r\nConnection: close\r\n\r\n' + body)
    s.sendall(req)
    data = b''
    while True:
        c = s.recv(4096)
        if not c: break
        data += c
    s.close()
    return json.loads(data.split(b'\r\n\r\n', 1)[1])

class ChatClient:
    def __init__(self, host='127.0.0.1', port=8990):
        self.sock = socket.create_connection((host, port), timeout=8)
        self.buf = b''
        self.uid = 0
    def send(self, msg_id, obj):
        payload = json.dumps(obj, ensure_ascii=False).encode()
        self.sock.sendall(struct.pack('>HH', msg_id, len(payload)) + payload)
    def recv(self, timeout=5):
        self.sock.settimeout(timeout)
        while len(self.buf) < 4:
            try:
                d = self.sock.recv(4096)
            except socket.timeout:
                return None
            if not d: return None
            self.buf += d
        mid, mlen = struct.unpack('>HH', self.buf[:4])
        while len(self.buf) < 4 + mlen:
            try:
                d = self.sock.recv(4096)
            except socket.timeout:
                return None
            if not d: return None
            self.buf += d
        msg = self.buf[4:4+mlen]
        self.buf = self.buf[4+mlen:]
        try:
            return mid, json.loads(msg.decode())
        except Exception:
            return mid, None
    def close(self):
        try: self.sock.close()
        except Exception: pass

def chat_login(client, uid, token):
    client.send(1005, {'uid': uid, 'token': token})
    while True:
        r = client.recv(8)
        if r is None: return False
        mid, obj = r
        if mid == 1006 and obj is not None and obj.get('uid') == uid:
            return obj.get('error', -1) == 0
    return False

def wait_for(client, want_id, timeout=8):
    deadline = time.time() + timeout
    while time.time() < deadline:
        r = client.recv(2)
        if r is None: continue
        mid, obj = r
        if mid == want_id and obj is not None:
            return obj
    return None

results = []
def check(name, ok, detail=''):
    results.append((name, ok, detail))
    print(('PASS' if ok else 'FAIL'), name, detail)

# ===== 1. 登录 =====
r = http_login('secondtonone1@163.com', 'ffli;89>?<')   # llfc 1002
check('llfc HTTP 登录', r.get('error') == 0, 'uid=%s' % r.get('uid'))
llfc_token = r['token']
r2 = http_login('2022202210033@whu.edu.cn', '~oy~;89>?<')  # test 1005
check('test HTTP 登录', r2.get('error') == 0, 'uid=%s' % r2.get('uid'))
test_token = r2['token']

llfc = ChatClient()
test = ChatClient()
check('llfc TCP 登录', chat_login(llfc, r['uid'], llfc_token))
check('test TCP 登录', chat_login(test, r2['uid'], test_token))
llfc.uid, test.uid = r['uid'], r2['uid']

# ===== 2. llfc 建群（含 test） =====
llfc.send(1032, {'uid': llfc.uid, 'name': '管理测试群', 'member_ids': [test.uid]})
rsp = wait_for(llfc, 1033)
thread_id = rsp.get('thread_id', 0) if rsp else 0
check('建群', rsp is not None and rsp.get('error') == 0 and thread_id > 0, 'thread=%s' % thread_id)

# ===== 3. 加载群成员（验证 group_nick/remark 字段） =====
llfc.send(1037, {'thread_id': thread_id})
rsp = wait_for(llfc, 1038)
members = rsp.get('members', []) if rsp else []
member_fields_ok = all('group_nick' in m and 'remark' in m for m in members)
check('加载成员含新字段', rsp is not None and len(members) >= 2 and member_fields_ok,
      'count=%s' % len(members))

# ===== 4. 群主设置 test 为管理员 (1048) =====
llfc.send(1048, {'uid': llfc.uid, 'thread_id': thread_id, 'target_uid': test.uid, 'role': 1})
rsp = wait_for(llfc, 1049)
check('群主设管理员回包', rsp is not None and rsp.get('error') == 0 and rsp.get('role') == 1)
notify = wait_for(test, 1050)
check('test 收到角色通知', notify is not None and notify.get('target_uid') == test.uid and notify.get('role') == 1)

# ===== 5. test 改群昵称 (1054) =====
test.send(1054, {'uid': test.uid, 'thread_id': thread_id, 'nick': '群昵称测试'})
rsp = wait_for(test, 1055)
check('test 改群昵称回包', rsp is not None and rsp.get('error') == 0 and rsp.get('nick') == '群昵称测试')

# ===== 6. test 改备注 (1056) =====
test.send(1056, {'uid': test.uid, 'thread_id': thread_id, 'remark': '备注测试'})
rsp = wait_for(test, 1057)
check('test 改备注回包', rsp is not None and rsp.get('error') == 0 and rsp.get('remark') == '备注测试')

# ===== 7. 群主改群头像 (1051) =====
icon_path = 'avatars/group_%d.png' % thread_id
llfc.send(1051, {'uid': llfc.uid, 'thread_id': thread_id, 'icon': icon_path})
rsp = wait_for(llfc, 1052)
check('群主改头像回包', rsp is not None and rsp.get('error') == 0 and rsp.get('icon') == icon_path)
notify = wait_for(test, 1053)
check('test 收到头像通知', notify is not None and notify.get('icon') == icon_path)

# ===== 8. 加载成员确认新值 =====
llfc.send(1037, {'thread_id': thread_id})
rsp = wait_for(llfc, 1038)
if rsp:
    for m in rsp.get('members', []):
        if m['uid'] == test.uid:
            check('成员 role=1/昵称/备注', m.get('role') == 1 and m.get('group_nick') == '群昵称测试'
                  and m.get('remark') == '备注测试', 'role=%s nick=%s remark=%s' % (m.get('role'), m.get('group_nick'), m.get('remark')))

# ===== 9. 普通权限：test 不能设管理员/改头像 =====
test.send(1048, {'uid': test.uid, 'thread_id': thread_id, 'target_uid': test.uid, 'role': 0})
rsp = wait_for(test, 1049)
check('非群主设管理员被拒', rsp is not None and rsp.get('error') == 1011)

# ===== 10. test 退出群聊 (1058)，群主收到通知 (1060) =====
test.send(1058, {'uid': test.uid, 'thread_id': thread_id})
rsp = wait_for(test, 1059)
check('test 退出群聊回包', rsp is not None and rsp.get('error') == 0)
notify = wait_for(llfc, 1060)
check('群主收到退出通知', notify is not None and notify.get('uid') == test.uid)

# ===== 11. 群主不能退出群聊 =====
llfc.send(1058, {'uid': llfc.uid, 'thread_id': thread_id})
rsp = wait_for(llfc, 1059)
check('群主退出被拒', rsp is not None and rsp.get('error') == 1011)

print()
print('===== 汇总 =====')
passed = sum(1 for _, ok, _ in results if ok)
print('通过 %d/%d' % (passed, len(results)))

# ===== 清理测试群数据 =====
llfc.close(); test.close()
print('thread_id=%s' % thread_id)
