import subprocess

class transition:
  def __init__(self, _backend):
    self.backend = _backend
    self.pipe = subprocess.Popen(_backend, bufsize = 1024, stdin = subprocess.PIPE, stdout = subprocess.PIPE)
  def write(self, str):
    self.pipe.stdin.write((str + '\n').encode(encoding = "UTF-8"))
    self.pipe.stdin.flush()
  def read(self):
    ret = ""
    while True:
      cur = self.pipe.stdout.readline().decode(encoding = "UTF-8")
      if (cur == "$$$$$$$$\n"):
        break
      else:
        ret += cur
    return ret
  def fetch(self, str):
    self.write(str)
    res = self.read()
    reslist = res.split("\n")
    #print(reslist) #FOR DEBUG
    return reslist
  
