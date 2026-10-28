@echo off
title 微信聊天系统 - 一键启动
echo ============================================
echo   微信聊天系统（llfcchat）一键启动脚本
echo ============================================
echo.

cd /d E:\myserve

rem 1. MySQL 3308（Windows 服务 MySQL267，通常开机自启；若未运行请手动启动服务）
sc query MySQL267 | findstr /i "RUNNING" >nul
if %errorlevel%==0 (
    echo [1/6] MySQL 3308 ...... 已在运行
) else (
    echo [1/6] MySQL 3308 ...... 未运行，尝试启动服务...
    net start MySQL267 >nul 2>&1
)

rem 2. Redis 6390
echo [2/6] Redis 6390 ...... 检查/启动
netstat -ano | findstr ":6390 " | findstr "LISTENING" >nul
if %errorlevel%==0 (
    echo        Redis 6390 已在运行
) else (
    start "Redis 6390" /D E:\myserve\Redis E:\myserve\Redis\redis-server.exe E:\myserve\Redis\redis.windows.conf
    timeout /t 2 /nobreak >nul
)

rem 3. VarifyServer (gRPC 50051)
echo [3/6] VarifyServer .... 启动
start "VarifyServer" /D E:\myserve\VarityServer E:\myserve\nodejs\node.exe server.js
timeout /t 2 /nobreak >nul

rem 4. GateServer (HTTP 8080)
echo [4/6] GateServer ...... 启动
start "GateServer" /D E:\myserve\gitserver\x64\Debug E:\myserve\gitserver\x64\Debug\gitserver.exe
timeout /t 2 /nobreak >nul

rem 5. StatusServer (gRPC 50052)
echo [5/6] StatusServer .... 启动
start "StatusServer" /D E:\myserve\server\StatusServer\x64\Debug E:\myserve\server\StatusServer\x64\Debug\StatusServer.exe
timeout /t 2 /nobreak >nul

rem 6. ChatServer1 (8990) 与 ChatServer2 (8991)
echo [6/6] ChatServer 1/2 .. 启动
start "ChatServer1" /D E:\myserve\server\ChatServer\x64\Debug E:\myserve\server\ChatServer\x64\Debug\ChatServer.exe
start "ChatServer2" /D E:\myserve\server\ChatServer2\x64\Debug E:\myserve\server\ChatServer2\x64\Debug\ChatServer2.exe

echo.
echo 等待服务启动...
timeout /t 5 /nobreak >nul

echo.
echo ============================================
echo   端口检查（应看到 8080/50051/50052/8990/8991/50055/50056）
echo ============================================
netstat -ano | findstr ":8080 :50051 :50052 :8990 :8991 :50055 :50056" | findstr "LISTENING"

echo.
echo 全部服务已启动！
echo 客户端：E:\myserve\herrychat\bin\herrychat.exe
echo 演示账号：llfc / 1002  （或 test / 1005）
echo.
pause
