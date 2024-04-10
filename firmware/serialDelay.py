import time

Import("env")

#if "test" in env.GetBuildType():
env.AddPostAction("upload", lambda *_, **__: time.sleep(5))