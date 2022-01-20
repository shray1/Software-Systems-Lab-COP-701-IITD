const express = require("express");
const mongoose = require("mongoose");
const bodyParser = require("body-parser");
const cors = require("cors");
const jwt = require("jsonwebtoken");
const secretKey = "lab5Chatapp";
const port = 5000;


const app = express();


const server = app.listen(port, () =>{
  console.log('Server running on port 5000.');
}
);
const io = require("socket.io")(server);

app.use(
  bodyParser.urlencoded({
    extended: false,
  })
);


// Refer https://stackoverflow.com/questions/29960764/what-does-extended-mean-in-express-4-0 
// for explanation on bodyParser -> extended feature.

app.use(bodyParser.json());
app.use(cors());
app.use(function (req, res, next) {
  req.io = io;
  next();
});
const dbUrl = "mongodb://localhost:27017/chatapp"

mongoose.connect(dbUrl, {
    useNewUrlParser: true,
    useUnifiedTopology: true,
  })
  .then(() => console.log("MongoDB Successfully Connected"))
  .catch((err) => console.log(err));

io.on( 'connection', function( socket ) {
  console.log('a user has connected!');
  socket.on('disconnect', function() {
    console.log('user disconnected');
  });
});



const PrivateMsgs = require('./models/PrivateMsgSchema');
const GroupMsgs = require('./models/GroupMsgSchema');
const Users = require('./models/UserSchema');
const Groups = require('./models/GroupSchema');



const getJWTTokenFromRequest = (req) => {
    if (req.headers.authorization &&
        req.headers.authorization.split(' ')[0] === 'Bearer'){
        return req.headers.authorization.split(' ')[1];
    }
    return null;
};

app.get('/', (req, res) => {
  //console.log(req.headers);
  res.send('Hello World!')
})

// Handling post request
app.post("/login", async(req, res) => {
  //console.log(req)
  let { userid, password } = req.body;
  //console.log(req.body);
  console.log("Login activity.");
  console.log(userid);
  console.log(password);
  var userExists = await Users.findOne({ userid:userid });
  //console.log(userExists);
  if (!userExists) {
    return res.status(401).json({ loginError : "Username not found" });
  }
  if ( userExists.password === password) {
        let payload = {id: userid};
        let token = jwt.sign(payload, secretKey,{expiresIn: 86400,}); 
        res.status(200).json({
              success: true,
              token:token,
              name: userExists.name,
              id: userid,
              privatechats:userExists.privatechats,
              groupchats:userExists.groupchats
        });
  }else{
    return res.status(401).json({ loginError: "Password incorrect" });
  }
});

//Users.findOne({ 'userid':'mcs212152' }).then((user) => {console.log(user);})
let currentUser = null;

// Token verfication middleware
app.use(function(req, res, next) {
    try {
        currentUser = jwt.verify(getJWTTokenFromRequest(req), secretKey);
        //console.log("Inside verification middleware.");
        //console.log(currentUser);
        next();
    } catch (err) {
        console.log(err);
        res.setHeader('Content-Type', 'application/json');
        res.end(JSON.stringify({ message: 'Unauthorized' }));
        res.sendStatus(401);
    }
});

app.post('/groupmsgs/', async (req, res) => {
    try {
      var {group_name} = req.body;
      var results = await GroupMsgs.find({group_name:group_name});
      //console.log(results);
      res.status(200).json({ groupMsgList: results,});
    }catch(err){
        //Add check for failure in above call.
        console.log(err);
        res.setHeader('Content-Type', 'application/json');
        res.end(JSON.stringify({ message: 'Failure' }));
        res.sendStatus(500);
    }
});


app.post('/privatemsgs/', async (req, res) => {
    //console.log("Inside PrivateMsgs API");
    try {
      var {from,to} = req.body;
      const results = await PrivateMsgs.find({$or : [{'from':from,'to':to},{'from':to,'to':from}]});
     
      //The results are already sorted!! 
      // results = results.sort(function (a, b) {
      //     var dateA = new Date(a.date_prop).getTime();
      //     var dateB = new Date(b.date_prop).getTime();
      //     return dateA < dateB ? 1 : -1; // ? -1 : 1 for ascending/increasing order
      // });
      //console.log(results);
      res.status(200).json({ privateMsgList: results});
    }catch(err){
        //Add check for failure in above call.
        console.log(err);
        res.setHeader('Content-Type', 'application/json');
        res.end(JSON.stringify({ message: 'Failure' }));
        res.sendStatus(500);
    }
});

app.post('/chatlist/',async(req,res) => {
  try {
      var {userid} = req.body;
      const result = await Users.findOne({'userid':userid});
      //console.log(result);
      res.status(200).json({ privateChatList:result.privatechats,groupChatList:result.groupchats});
    }catch(err){
        //Add check for failure in above call.
        console.log(err);
        res.setHeader('Content-Type', 'application/json');
        res.end(JSON.stringify({ message: 'Failure' }));
        res.sendStatus(500);
    }
});

app.get('/allgroupnames/',async(req,res) => {
  try {
      const result = await Groups.find().distinct("name");
      //console.log(result);
      res.status(200).json({ groupNames:result});
    }catch(err){
        //Add check for failure in above call.
        console.log(err);
        res.setHeader('Content-Type', 'application/json');
        res.end(JSON.stringify({ message: 'Failure' }));
        res.sendStatus(500);
    }
});


app.post('/newgroupmsg/', async(req,res) =>{
    var {from,group_name,body}= req.body;
    var newMessage = new GroupMsgs({from:from,group_name:group_name,date:Date(),body:body});
    try{
      newMessage.save();
      
      const jsonMsg = ({from:from,group_name:group_name,body:body,type:"group"})
      //req.io.sockets.emit('messages', jsonMsg);
      io.sockets.emit('updatechat',jsonMsg);
      
      res.setHeader('Content-Type', 'application/json');
      res.end(JSON.stringify({ message: 'Success' }));
    }catch(err){
      console.log(err);
      res.setHeader('Content-Type', 'application/json');
      res.end(JSON.stringify({ message: 'Failure' }));
      res.sendStatus(500);
    }
});

app.post('/newprivatemsg/', async(req,res) =>{
    var {from,to,body}= req.body;
    var newMessage = new PrivateMsgs({from:from,to:to,date:Date(),body:body});
    try{
      newMessage.save();
      
      const jsonMsg = ({from:from,to:to,body:body,date:Date(),type:"private"})
      //req.io.sockets.emit('messages', jsonMsg);
      //io.sockets.emit('message',{});
      io.sockets.emit('updatechat',jsonMsg);
      res.setHeader('Content-Type', 'application/json');
      res.end(JSON.stringify({ message: 'Success' }));
    }catch(err){
      console.log(err);
      res.setHeader('Content-Type', 'application/json');
      res.end(JSON.stringify({ message: 'Failure' }));
      res.sendStatus(500);
    }
});

// app.get('/privatechats/', async (req, res) => {
//     try {
//       var {userid} = req.body;
//       var resultsFrom = await PrivateMsgs.find({from:userid});
//       var resultsTo = await PrivateMsgs.find({to:userid});
      
//       //console.log(results);
//       res.status(200).json({ privateMsgList: results,});
//     }catch(err){
//         //Add check for failure in above call.
//         console.log(err);
//         res.setHeader('Content-Type', 'application/json');
//         res.end(JSON.stringify({ message: 'Failure' }));
//         res.sendStatus(500);
//     }
// });

//Users.find().then(results => console.log(results));

// var gmsg = new GroupMsgs({from:'mcs212152',group_name:'bindaas',date:Date(),body:"Sabko Radhaswami bhailog!"})
// console.log(gmsg);
// gmsg.save();


//var pmsg = new PrivateMsgs({from:'mcs212151',to:'mcs212152',date:Date(),body:"Radhaswami Shray ji :D"})
//console.log(pmsg);
//pmsg.save();