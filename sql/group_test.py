# -*- coding: utf-8 -*-
"""群聊端到端协议联调：llfc(1002) 建群 -> 拉 test(1005) -> 群消息 -> 改群名 -> 移除"""
import socket, struct, json, time, threading

GATE = ("127.0.0.1", 8080)
CHAT = ("127.0.0.1", 8990)

def http_login(email, pwd_cipher):
    body = json.dumps({"email": email, "passwd": pwd_cipher}).encode("utf-8")
    s = socket.create_connection(GATE, timeout=5)
    req = (b"POST /user_login HTTP/1.1\r\nHost: 127.0.0.1\r\nContent-Type: application/json\r\n"
           b"Content-Length: " + str(len(body)).encode() + b"\r\nConnection: close\r\n\r\n" + body)
    s.sendall(req)
    data = b""
    while True:
        chunk = s.recv(4096)
        if not chunk: break
        data += chunk
    s.close()
    body_part = data.split(b"\r\n\r\n", 1)[1]
    return json.loads(body_part.decode("utf-8"))

class TcpClient:
    def __init__(self, name):
        self.name = name
        self.sock = socket.create_connection(CHAT, timeout=10)
        self.buf = b""
        self.msgs = []
        self.lock = threading.Lock()
        self.thread = threading.Thread(target=self._recv_loop, daemon=True)
        self.thread.start()
    def _recv_loop(self):
        try:
            while True:
                chunk = self.sock.recv(4096)
                if not chunk: break
                self.buf += chunk
                while len(self.buf) >= 4:
                    msg_id, msg_len = struct.unpack(">HH", self.buf[:4])
                    if len(self.buf) < 4 + msg_len: break
                    payload = self.buf[4:4+msg_len]
                    self.buf = self.buf[4+msg_len:]
                    try:
                        obj = json.loads(payload.decode("utf-8"))
                    except Exception as e:
                        obj = {"raw": payload.decode("utf-8", "ignore"), "parse_err": str(e)}
                    with self.lock:
                        self.msgs.append((msg_id, obj))
        except Exception as e:
            pass
    def send(self, msg_id, obj):
        payload = json.dumps(obj, ensure_ascii=False).encode("utf-8")
        self.sock.sendall(struct.pack(">HH", msg_id, len(payload)) + payload)
    def wait(self, msg_id, timeout=8, clear=True):
        """等待指定 msg_id 的包"""
        deadline = time.time() + timeout
        while time.time() < deadline:
            with self.lock:
                for i, (mid, obj) in enumerate(self.msgs):
                    if mid == msg_id:
                        if clear:
                            del self.msgs[i]
                        return obj
            time.sleep(0.1)
        return None
    def close(self):
        try: self.sock.close()
        except: pass

def chat_login(cli, uid, token):
    cli.send(1005, {"uid": uid, "token": token})
    rsp = cli.wait(1006, timeout=8)
    return rsp

def main():
    results = []

    # 1. HTTP 登录两个账号
    llfc = http_login("secondtonone1@163.com", "ffli;89>?<")
    test = http_login("2022202210033@whu.edu.cn", "~oy~;89>?<")
    results.append(("HTTP登录 llfc", llfc))
    results.append(("HTTP登录 test", test))
    if llfc.get("error") != 0 or test.get("error") != 0:
        print("HTTP 登录失败，终止"); return results
    uid_llfc, tok_llfc = llfc["uid"], llfc["token"]
    uid_test, tok_test = test["uid"], test["token"]

    # 2. TCP 登录
    c1 = TcpClient("llfc"); c2 = TcpClient("test")
    r1 = chat_login(c1, uid_llfc, tok_llfc)
    r2 = chat_login(c2, uid_test, tok_test)
    results.append(("TCP登录 llfc error", r1.get("error") if r1 else "TIMEOUT"))
    results.append(("TCP登录 test error", r2.get("error") if r2 else "TIMEOUT"))
    results.append(("llfc 登录回包 group_list", r1.get("group_list") if r1 else None))

    # 3. llfc 建群，拉 test
    c1.send(1032, {"uid": uid_llfc, "name": "测试群", "member_ids": [uid_test]})
    create_rsp = c1.wait(1033, timeout=8)
    results.append(("建群回包 1033", create_rsp))
    notify_join = c2.wait(1041, timeout=8)
    results.append(("test 收到拉人通知 1041", notify_join))
    # 打印 test 收到的所有消息，观察 1041 是否真的到达
    with c2.lock:
        results.append(("test 全部收到的包", [ (m, o) for m, o in c2.msgs ]))
    if not create_rsp or create_rsp.get("error") != 0:
        print("建群失败，终止"); return results
    thread_id = create_rsp["thread_id"]

    # 4. test 发群消息
    c2.send(1034, {"fromuid": uid_test, "touid": 0, "thread_id": thread_id,
                   "text_array": [{"content": "大家好，我是test", "unique_id": "u-test-001"}]})
    rsp_1035 = c2.wait(1035, timeout=8)
    results.append(("test 群消息回包 1035", rsp_1035))
    notify_1036 = c1.wait(1036, timeout=8)
    results.append(("llfc 收到群消息通知 1036", notify_1036))

    # 5. llfc 回群消息
    c1.send(1034, {"fromuid": uid_llfc, "touid": 0, "thread_id": thread_id,
                   "text_array": [{"content": "欢迎欢迎", "unique_id": "u-llfc-002"}]})
    n2 = c2.wait(1036, timeout=8)
    results.append(("test 收到 llfc 群消息 1036", n2))

    # 6. llfc 加载群成员
    c1.send(1037, {"thread_id": thread_id})
    m = c1.wait(1038, timeout=8)
    results.append(("加载群成员 1038", m))

    # 7. llfc 改群名
    c1.send(1045, {"uid": uid_llfc, "thread_id": thread_id, "name": "新群名"})
    rsp_1046 = c1.wait(1046, timeout=8)
    results.append(("改群名回包 1046", rsp_1046))
    n_1047_c1 = c1.wait(1047, timeout=8)
    n_1047_c2 = c2.wait(1047, timeout=8)
    results.append(("llfc 收到改名通知 1047", n_1047_c1))
    results.append(("test 收到改名通知 1047", n_1047_c2))

    # 8. llfc 移除 test
    c1.send(1042, {"uid": uid_llfc, "thread_id": thread_id, "target_uid": uid_test})
    rsp_1043 = c1.wait(1043, timeout=8)
    results.append(("移除回包 1043", rsp_1043))
    n_1044 = c2.wait(1044, timeout=8)
    results.append(("test 收到被移除通知 1044", n_1044))

    # 9. 普通成员 test 尝试移除 llfc（应失败）
    c2.send(1042, {"uid": uid_test, "thread_id": thread_id, "target_uid": uid_llfc})
    rsp_fail = c2.wait(1043, timeout=8)
    results.append(("普通成员移除应失败 1043", rsp_fail))

    c1.close(); c2.close()
    return results

if __name__ == "__main__":
    for k, v in main():
        print("=====", k, "=====")
        print(json.dumps(v, ensure_ascii=False, indent=1) if not isinstance(v, str) else v)
