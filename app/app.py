#具体报错信息未添加
from flask import Flask, render_template, request, flash, url_for, redirect
from transitioner import transition
import time
import json

app = Flask(__name__)
app.secret_key = "Polaris_Dane"

now_usr = ""

now_p = 0

inter = transition("../code")

@app.route("/")
def home():
  global now_usr
  global now_p
  msg = request.args.get("msg")
  Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
  message = "[{}] add_user -c Polaris_Dane -u Polaris_Dane -p 5y57576 -n 陈一星 -m 2488721971@qq.com -g 10".format(Timetag)
  reslist = inter.fetch(message)
  return render_template("home.html", now_usr = now_usr, msg = msg)
  
@app.route("/login", methods = ["GET", "POST"])
def login():
  global now_usr
  global now_p
  if request.method == "GET":
    return render_template("login.html", now_usr = now_usr)
  if request.method == "POST":
    UserName = request.form.get("UserName")
    UserPassword = request.form.get("UserPassword")
    Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
    message = "[{}] login -u {} -p {}".format(Timetag, UserName, UserPassword)
    reslist = inter.fetch(message)
    print(reslist)
    res = reslist[0].split(" ")
    Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
    message = "[{}] query_profile -c {} -u {}".format(Timetag, UserName, UserName)
    reslist = inter.fetch(message)
    pri = reslist[0].split(" ")
    if res[1] == "0":
      now_usr = UserName
      now_p = int(pri[4])
      #print(now_p)
      return json.dumps({"status": "0", "username": UserName})
    else:
      return json.dumps({"status": "-1", "username": ""})#还未传报错信息
@app.route("/add_user", methods=["GET", "POST"])
def add_user():
  #密码确认保密未进行
  global now_usr
  global now_p
  if now_usr == "":
    msg = "Not logined"
    return redirect(url_for("home", msg = msg))
  elif now_p < 9:
    msg = "Authority not enough"
    return redirect(url_for("home", msg = msg))
  else:
    if request.method == "GET":
      return render_template("add_user.html", now_usr = now_usr, now_p = now_p)
    if request.method == "POST":
      Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
      newName = request.form.get("newName")
      newPassword = request.form.get("newPassword")
      newRealname = request.form.get("newRealname")
      newMail = request.form.get("newMail")
      newP = request.form.get("newP")
      message = "[{}] add_user -c {} -u {} -p {} -n {} -m {} -g {}".format(Timetag, now_usr, newName, newPassword, newRealname, newMail, newP)
      #print(message)
      reslist = inter.fetch(message)
      #print(reslist)
      res = reslist[0].split(" ")
      if res[1] == "0":
        return json.dumps({"status": "0"})
      else:
        return json.dumps({"status": "-1"})

@app.route("/user", methods = ["GET", "POST"])
def user():
  global now_usr
  global now_ps
  if now_usr != "":
    return render_template("user.html", now_usr = now_usr, now_p = now_p)
  else:
    flash("Not Logined", "alert")
    return redirect(url_for("login"))
    
@app.route("/logout", methods = ["GET", "POST"])
def logout():
  global now_usr
  global now_p
  if now_usr == "":
    msg = "Not logined"
    return redirect(url_for("home", msg = msg))
  else:
    Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
    message = "[{}] logout -u {}".format(Timetag, now_usr)
    inter.fetch(message)
    now_usr = ""
    now_p = 0
    msg = "Logout success"
    return redirect(url_for("home", msg = msg))

@app.route("/query_profile_admin", methods = ["GET", "POST"])
def query_profile_admin():
  global now_usr
  global now_p
  if now_usr == "":
    msg = "Not logined"
    return redirect(url_for("home", msg = msg))
  elif now_p < 9:
    msg = "Authority not enough"
    return redirect(url_for("home", msg = msg))
  else:
    if request.method == "GET":
      return render_template("query_profile_admin.html", now_usr = now_usr)
    if request.method == "POST":
      UserName = request.form.get("UserName")
      Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
      message = "[{}] query_profile -u {} -c {}".format(Timetag, UserName, now_usr)
      reslist = inter.fetch(message)
      res = reslist[0].split(" ")
      if res[1] == "-1":
        return json.dumps({"status": "-1", "username": "", "userrealname": "", "usermail": "", "userp": ""})
      else:
        return json.dumps({"status": "0", "username": res[1], "userrealname": res[2], "usermail": res[3], "userp": res[4]})

@app.route("/query_profile_user", methods = ["GET", "POST"])
def query_profile_user():
  global now_usr
  global now_p
  if now_usr == "":
    msg = "Not logined"
    return redirect(url_for("home", msg = msg))
  else:
    if request.method == "GET":
      Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
      message = "[{}] query_profile -u {} -c {}".format(Timetag, now_usr, now_usr)
      reslist = inter.fetch(message)
      #print(reslist)
      res = reslist[0].split(" ")
      #print(res[1])
      return render_template("query_profile_user.html", now_usr = now_usr, username = res[1], userrealname = res[2], usermail = res[3], userp = res[4])

@app.route("/modify_profile_admin", methods = ["GET", "POST"])
def modify_profile_admin():
  #密码确认保密未进行
  global now_usr
  global now_p
  if now_usr == "":
    msg = "Not logined"
    return redirect(url_for("home", msg = msg))
  elif now_p < 9:
    msg = "Authority not enough"
    return redirect(url_for("home", msg = msg))
  else:
    if request.method == "GET":
      return render_template("modify_profile_admin.html", now_usr = now_usr)
    if request.method == "POST":
      UserName = request.form.get("UserName")
      newPassword = request.form.get("newPassword")
      newRealname = request.form.get("newRealname")
      newMail = request.form.get("newMail")
      newP = request.form.get("newP")
      Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
      message = "[{}] modify_profile -c {} -u {} ".format(Timetag, now_usr, UserName)
      if newPassword != "":
        message += "-p {} ".format(newPassword)
      if newRealname != "":
        message += "-n {} ".format(newRealname)
      if newMail != "":
        message += "-m {} ".format(newMail)
      if newP != "-1":
        message += "-g {} ".format(newP)
      #print(message)
      reslist = inter.fetch(message)
      #print(reslist)
      res = reslist[0].split(" ")
      if res[1] == "-1":
        return json.dumps({"status": "-1"})
      else:
        return json.dumps({"status": "0"})

@app.route("/modify_profile_user", methods = ["GET", "POST"])
def modify_profile_user():
  #密码确认保密未进行
  global now_usr
  global now_p
  if now_usr == "":
    msg = "Not logined"
    return redirect(url_for("home", msg = msg))
  else:
    if request.method == "GET":
      return render_template("modify_profile_user.html", now_usr = now_usr)
    if request.method == "POST":
      newPassword = request.form.get("newPassword")
      newRealname = request.form.get("newRealname")
      newMail = request.form.get("newMail")
      Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
      message = "[{}] modify_profile -c {} -u {} ".format(Timetag, now_usr, now_usr)
      if newPassword != "":
        message += "-p {} ".format(newPassword)
      if newRealname != "":
        message += "-n {} ".format(newRealname)
      if newMail != "":
        message += "-m {} ".format(newMail)
      #print(message)
      reslist = inter.fetch(message)
      #print(reslist)
      res = reslist[0].split(" ")
      if res[1] == "-1":
        return json.dumps({"status": "-1"})
      else:
        return json.dumps({"status": "0"})

@app.route("/add_train", methods = ["GET", "POST"])
def add_train():
  global now_usr
  global now_p
  if now_usr == "":
    msg = "Not logined"
    return redirect(url_for("home", msg = msg))
  elif now_p < 9:
    msg = "Authority not enough"
    return redirect(url_for("home", msg = msg))
  else:
    if request.method == "GET":
      return render_template("add_train.html", now_usr = now_usr)
    if request.method == "POST":
      newID = request.form.get("newID")
      newstationNum = request.form.get("newstationNum")
      newseatNum = request.form.get("newseatNum")
      stations = request.form.get("stations")
      price = request.form.get("price")
      new_st_time = request.form.get("new_st_time")
      new_trav_time = request.form.get("new_trav_time")
      new_stop_time = request.form.get("new_stop_time")
      newsaleDate = request.form.get("newsaleDate")
      newType = request.form.get("newType")
      Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
      message = "[{}] add_train -i {} -n {} -m {} -s {} -p {} -x {} -t {} -o {} -d {} -y {}".format(Timetag, newID, newstationNum, newseatNum, stations, price, new_st_time, new_trav_time, new_stop_time, newsaleDate, newType)
      print(message)
      reslist = inter.fetch(message)
      res = reslist[0].split(" ")
      if res[1] == "-1":
        return json.dumps({"status": "-1"})
      else:
        return json.dumps({"status": "0"})

@app.route("/delete_train", methods = ["GET", "POST"])
def delete_train():
  global now_usr
  global now_p
  if now_usr == "":
    msg = "Not logined"
    return redirect(url_for("home", msg = msg))
  elif now_p < 9:
    msg = "Authority not enough"
    return redirect(url_for("home", msg = msg))
  else:
    if request.method == "GET":
      return render_template("delete_train.html", now_usr = now_usr)
    if request.method == "POST":
      trainID = request.form.get("trainID")
      Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
      message = "[{}] delete_train -i {}".format(Timetag, trainID)
      #print(message)
      reslist = inter.fetch(message)
      #print(reslist)
      res = reslist[0].split(" ")
      if res[1] == "-1":
        return json.dumps({"status": "-1"})
      else:
        return json.dumps({"status": "0"})

@app.route("/release_train", methods = ["GET", "POST"])
def release_train():
  global now_usr
  global now_p
  if now_usr == "":
    msg = "Not logined"
    return redirect(url_for("home", msg = msg))
  elif now_p < 9:
    msg = "Authority not enough"
    return redirect(url_for("home", msg = msg))
  else:
    if request.method == "GET":
      return render_template("release_train.html", now_usr = now_usr)
    if request.method == "POST":
      trainID = request.form.get("trainID")
      Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
      message = "[{}] release_train -i {}".format(Timetag, trainID)
      #print(message)
      reslist = inter.fetch(message)
      #print(reslist)
      res = reslist[0].split(" ")
      if res[1] == "-1":
        return json.dumps({"status": "-1"})
      else:
        return json.dumps({"status": "0"})

@app.route("/query_train", methods = ["GET", "POST"])
def query_train():
  global now_usr
  global now_p
  if now_usr == "":
    msg = "Not logined"
    return redirect(url_for("home", msg = msg))
  else:
    if request.method == "GET":
      return render_template("query_train.html", now_usr = now_usr)
    if request.method == "POST":
      trainID = request.form.get("trainID")
      Date = request.form.get("Date")
      Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
      message = "[{}] query_train -i {} -d {}".format(Timetag, trainID, Date)
      #print(message)
      reslist = inter.fetch(message)
      #print(reslist)
      res = reslist[0].split(" ")
      if res[1] == "-1":
        return json.dumps({"status": "-1"})
      else:
        #return json.dumps({"status": "0", "data": reslist})
        Type = res[2]
        stations = []
        reslen = len(reslist)
        for i in range(1, reslen - 1):
          res = reslist[i].split(" ")
          station = {"name": res[0], "arrive": res[1] + " " + res[2], "leave": res[4] + " " + res[5], "price": res[6], "seat": res[7]}
          #print(station)
          stations.append(station)
        return json.dumps({"status": "0", "trainID": trainID, "type": Type, "stations": stations})

@app.route("/query_ticket", methods = ["GET", "POST"])
def query_ticket():
  global now_usr
  global now_p
  if now_usr == "":
    msg = "Not logined"
    return redirect(url_for("home", msg = msg))
  else:
    if request.method == "GET":
      return render_template("query_ticket.html", now_usr = now_usr)
    if request.method == "POST":
      st_sta = request.form.get("st_sta")
      ed_sta = request.form.get("ed_sta")
      Date = request.form.get("Date")
      opt = request.form.get("opt")
      Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
      message = "[{}] query_ticket -s {} -t {} -d {} -p {}".format(Timetag, st_sta, ed_sta, Date, opt)
      print(message)
      reslist = inter.fetch(message)
      print(reslist)
      res = reslist[0].split(" ")
      if res[1] == "-1":
        return json.dumps({"status": "-1"})
      else:
        tickets = []
        reslen = len(reslist)
        for i in range(1, reslen - 1):
          res = reslist[i].split(" ")
          ticket = {"name": res[0], "leave": res[2] + " " + res[3], "arrive": res[6] + " " + res[7], "price": res[8], "seat": res[9]}
          tickets.append(ticket)
        return json.dumps({"status": "0", "tickets": tickets})

@app.route("/query_transfer", methods = ["GET", "POST"])
def query_transfer():
  global now_usr
  global now_p
  if now_usr == "":
    msg = "Not logined"
    return redirect(url_for("home", msg = msg))
  else:
    if request.method == "GET":
      return render_template("query_transfer.html", now_usr = now_usr)
    if request.method == "POST":
      st_sta = request.form.get("st_sta")
      ed_sta = request.form.get("ed_sta")
      Date = request.form.get("Date")
      opt = request.form.get("opt")
      Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
      message = "[{}] query_transfer -s {} -t {} -d {} -p {}".format(Timetag, st_sta, ed_sta, Date, opt)
      #print(message)
      reslist = inter.fetch(message)
      #print(reslist)
      res = reslist[0].split(" ")
      if res[1] == "-1":
        return json.dumps({"status": "-1"})
      elif res[1] == "0":
        return json.dumps({"status": "1"})
      else:
        tickets = []
        res = reslist[0].split(" ")
        ticket1 = {"name": res[1], "leave": res[3] + " " + res[4], "arrive": res[7] + " " + res[8], "price": res[9], "seat": res[10]}
        tickets.append(ticket1)
        inter_sta = res[6]
        res = reslist[1].split(" ")
        ticket2 = {"name": res[0], "leave": res[2] + " " + res[3], "arrive": res[6] + " " + res[7], "price": res[8], "seat": res[9]}
        tickets.append(ticket2)
        return json.dumps({"status": "0", "tickets": tickets, "inter_sta": inter_sta})

@app.route("/buy_ticket", methods = ["GET", "POST"])
def buy_ticket():
  global now_usr
  global now_p
  if now_usr == "":
    msg = "Not logined"
    return redirect(url_for("home", msg = msg))
  else:
    if request.method == "GET":
      return render_template("buy_ticket.html", now_usr = now_usr)
    if request.method == "POST":
      trainID = request.form.get("trainID")
      ticketNum = request.form.get("ticketNum")
      st_sta = request.form.get("st_sta")
      ed_sta = request.form.get("ed_sta")
      Date = request.form.get("Date")
      opt = request.form.get("opt")
      Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
      message = "[{}] buy_ticket -u {} -i {} -n {} -f {} -t {} -d {} -q {}".format(Timetag, now_usr, trainID, ticketNum, st_sta, ed_sta, Date, opt)
      print(message)
      reslist = inter.fetch(message)
      print(reslist)
      res = reslist[0].split(" ")
      if res[1] == "-1":
        return json.dumps({"status": "-1"})
      elif res[1] == "queue":
        return json.dumps({"status": "1"})
      else:
        return json.dumps({"status": "0"})

if __name__ == "__main__":
  #Timetag = time.strftime("%Y-%m-%d;%H:%M:%S", time.localtime(time.time()))
  #message = "[{}] add_user -c Polaris_Dane -u Polaris_Dane -p 5y57576 -n 陈一星 -m 2488721971@qq.com -g 10".format(Timetag)
  #reslist = inter.fetch(message)
  app.run()
