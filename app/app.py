# TO-DO LIST:
# 1.响应式布局！！！
from flask import Flask, render_template, request, flash, url_for, redirect
from transitioner import transition
import time
import json

app = Flask(__name__)
app.secret_key = "Polaris_Dane"

now_usr = ""

inter = transition("../code")

@app.route("/")
def home():
  msg = request.args.get("msg")
  return render_template("home.html", now_usr = now_usr, msg = msg)
  
@app.route("/login", methods = ["GET", "POST"])
def login():
  global now_usr
  if request.method == "GET":
    return render_template("login.html", now_usr = now_usr)
  if request.method == "POST":
    Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
    UserName = request.form.get("UserName")
    UserPassword = request.form.get("UserPassword")
    message = "[{}] login -u {} -p {}".format(Timetag, UserName, UserPassword)
    reslist = inter.fetch(message)
    print(reslist)
    res = reslist[0].split(" ")
    if res[1] == "0":
      now_usr = UserName
      return json.dumps({"status": "0", "username": UserName})
    else:
      return json.dumps({"status": "-1", "username": ""})#还未传报错信息

#@app.route("/add_usr", methods=["GET", "POST"])
#def add_usr():
#  global now_usr
#  if request.method == "GET":
#    return render_template("add_usr.html")
#  if request.method == "POST":
#    Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time))
#    newName = request.form.get("newName")
#    newPassword = request.form.get("newPassword")
#    newRealname = request.form.get("newRealname")
#    newMail = request.form.get("newMail")
#    newP = request.form.get("newP")
#    message = "[{}] add_user -c {} -u {} -p {} -n {} -m {} -g {}".format(Timetag, newName, newPassword, newRealname, newMail, newP)
#    reslist = inter.fetch(message)
#    res = reslist[1]
#    if res == "0":
#      #whatever
#    else:
#      #res[2]为报错信息
@app.route("/user", methods = ["GET", "POST"])
def user():
  global now_usr
  if now_usr != "":
    return render_template("user.html", now_usr = now_usr)
  else:
    flash("Not Logined", "alert")
    return redirect(url_for("login"))
    
@app.route("/logout", methods=["GET", "POST"])
def logout():
  global now_usr
  if now_usr == "":
    msg = "Not logined"
    return redirect(url_for("home", msg = msg))
  else:
    Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
    message = "[{}] logout -u {}".format(Timetag, now_usr)
    inter.fetch(message)
    now_usr = ""
    msg = "Logout success"
    return redirect(url_for("home", msg = msg))

@app.route("/profile", methods=["GET", "POST"])
def profile():
  global now_usr
  if request.method == "GET":
    if now_usr != "":
      return render_template("profile.html", user_name = now_user)
    else:
      flash("Not Logined", "alert")
      return redirect(url_for("home"))
  if request.method == "POST":


if __name__ == "__main__":
  app.run()
