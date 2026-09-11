#!/bin/bash

# ================= 配置区域 =================
BIN="./epoll_server"   # 编译出的可执行文件名字
PORT="8080"            # 默认监听端口
PID_FILE="server.pid"  # 记录当前运行进程 PID 的文件
# ============================================

# 启动服务
start() {
    # 1. 检查二进制文件是否存在
    if [ ! -f "$BIN" ]; then
        echo "[ERROR] 可执行文件 $BIN 不存在，请先执行 make 编译！"
        exit 1
    fi

    # 2. 检查端口是否被占用
    PID_PORT=$(lsof -t -i:"$PORT" 2>/dev/null)
    if [ -n "$PID_PORT" ]; then
        echo "[WARN] 端口 $PORT 已被占用 (PID: $PID_PORT)，服务可能已在运行或端口冲突！"
        return
    fi

    echo "正在启动 $BIN (Port: $PORT) 守护进程..."
    $BIN "$PORT"

    # 等待进程启动并在后台稳定
    sleep 0.5

    # 3. 获取刚启动的守护进程 PID 并持久化
    PID=$(lsof -t -i:"$PORT" 2>/dev/null)
    if [ -n "$PID" ]; then
        echo "$PID" > "$PID_FILE"
        echo "[SUCCESS] 服务启动成功！PID: $PID, 监听端口: $PORT"
    else
        echo "[ERROR] 服务启动失败，请检查日志或配置！"
    fi
}

# 停止服务
stop() {
    # 优先从端口和进程名抓取精确 PID，防止误杀
    PID=$(lsof -t -i:"$PORT" 2>/dev/null)
    
    if [ -z "$PID" ] && [ -f "$PID_FILE" ]; then
        PID=$(cat "$PID_FILE")
    fi

    if [ -z "$PID" ]; then
        echo "[INFO] 没有发现运行在端口 $PORT 的服务进程。"
        rm -f "$PID_FILE"
        return
    fi

    echo "正在停止服务 (PID: $PID)..."
    kill -15 "$PID" 2>/dev/null

    # 循环等待进程优雅退出，最多等待 3 秒
    for i in {1..6}; do
        if ! kill -0 "$PID" 2>/dev/null; then
            break
        fi
        sleep 0.5
    done

    # 若进程仍未退出，则强制杀死
    if kill -0 "$PID" 2>/dev/null; then
        echo "[WARN] 进程未及时退出，执行强制终止 (kill -9)..."
        kill -9 "$PID" 2>/dev/null
    fi

    rm -f "$PID_FILE"
    echo "[SUCCESS] 服务已停止。"
}

# 查看状态
status() {
    PID=$(lsof -t -i:"$PORT" 2>/dev/null)
    if [ -n "$PID" ]; then
        echo "[RUNNING] 服务正在运行中。"
        echo "  - 进程 PID: $PID"
        echo "  - 监听端口: $PORT"
        echo "  - 进程信息: $(ps -p "$PID" -o comm=)"
    else
        echo "[STOPPED] 服务未运行。"
    fi
}

# 路由分发
case "$1" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    restart)
        stop
        sleep 1
        start
        ;;
    status)
        status
        ;;
    *)
        echo "用法: $0 {start|stop|restart|status}"
        exit 1
        ;;
esac