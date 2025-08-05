# re-broadcaster_v4.py
import asyncio, os, sys, websockets, argparse

clients = set()

async def ws_handler(ws):
    clients.add(ws)
    try:
        await ws.wait_closed()
    finally:
        clients.discard(ws)

async def fanout(line: str):
    dead = []
    for ws in list(clients):
        try:
            await ws.send(line)
        except Exception:
            dead.append(ws)
    for ws in dead:
        clients.discard(ws)

def reader_thread(pipe_path: str, loop: asyncio.AbstractEventLoop, q: asyncio.Queue):
    # Blocks reading the FIFO; push lines into the async queue from this thread
    while True:
        try:
            with open(pipe_path, "r") as f:
                for line in f:
                    loop.call_soon_threadsafe(q.put_nowait, line.rstrip("\n"))
        except FileNotFoundError:
            # FIFO not there yet; try again shortly
            import time; time.sleep(0.2)
        except Exception:
            import time; time.sleep(0.1)

async def queue_pump(q: asyncio.Queue):
    while True:
        line = await q.get()
        await fanout(line)

async def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--pipe", default="/tmp/litehaus_pipe")
    ap.add_argument("--host", default="0.0.0.0")
    ap.add_argument("--port", type=int, default=8083)
    args = ap.parse_args()

    # Ensure FIFO exists
    if not os.path.exists(args.pipe):
        os.mkfifo(args.pipe)

    loop = asyncio.get_running_loop()
    q = asyncio.Queue()

    # Start FIFO reader in a thread, passing THIS loop
    import threading
    t = threading.Thread(target=reader_thread, args=(args.pipe, loop, q), daemon=True)
    t.start()

    print(f"[*] WebSocket server up on ws://{args.host}:{args.port}")
    async with websockets.serve(ws_handler, args.host, args.port):
        await queue_pump(q)

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        pass
