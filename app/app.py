# TO-DO LIST:
# 1.响应式布局！！！

from flask import Flask, render_template, request, flash, url_for, redirect
from transitioner import transition
import json

app = Flask(__name__)
app.secret_key = "Polaris_Dane"

now_usr = ""

inter = transition("./backend")

@app.route("/")
def home():
  return render_template("home.html")
  
@app.route("/login", methods=["GET", "POST"])
def login():
  global now_usr
  if request.method == "GET":
    return render_template("login.html")
  if request.method == "POST":
    usr_name = request.form.get("usr_name")
    usr_password = request.form.get("usr_password")
    message = "1 {} {}".format(usr_name, usr_password)
    reslist = inter.login(message)
    res = reslist[0]
    if res == "1":
      now_usr = usr_name
      print(now_usr)
      return "1"
    else:
      return "-1"

@app.route("/user", methods=["GET", "POST"])
def user():
  global now_usr
  if now_usr != "":
    return render_template("user.html", usr_name = now_usr)
  else:
    flash("Not Logined", "alert")
    return redirect(url_for("home"))
    
@app.route("/logout", methods=["GET", "POST"])
def logout():
  global now_usr
  if now_usr == "":
    flash("Not Logined", "alert")
    return redirect(url_for("home"))
  else:
    now_usr = ""
    flash("Logout success", "info")
    return redirect(url_for("home"))
      
if __name__ == "__main__":
  app.run()
