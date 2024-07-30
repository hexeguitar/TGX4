import time

Import("env")

env.AddPostAction("upload", lambda *_, **__: time.sleep(5))
