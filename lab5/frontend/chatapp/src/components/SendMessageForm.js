import React from 'react';
//import PropTypes from 'prop-types';
import { useState } from 'react';
import Grid from "@material-ui/core/Grid";
import TextField from "@material-ui/core/TextField";
import IconButton from "@material-ui/core/IconButton";
import SendIcon from "@material-ui/icons/Send";
import "./chat.css";


const SendMessageForm = ({ selectedChat, currentUser, setMessageList, messageList,groupList }) => {
  const [errorMessage, setErrorMessage] = useState('');
  const postPrivateMsg = async (newMsg) => {
    const response = await fetch('http://localhost:5000/newprivatemsg/', {
       method: 'POST',
       headers: {
         'Content-Type': 'application/json',
         'Authorization': "Bearer " + currentUser.token 
       },
       body: JSON.stringify(newMsg)
     });
    return response.json();
  }
  const postGroupMsg = async (newMsg) => {
    const response = await fetch('http://localhost:5000/newgroupmsg/', {
       method: 'POST',
       headers: {
         'Content-Type': 'application/json',
         'Authorization': "Bearer " + currentUser.token 
       },
       body: JSON.stringify(newMsg)
     });
    return response.json();
  }
  const handleSendNewMsg = async (e) => {
    e.preventDefault();
    setErrorMessage('');
    const newMsgBody = (e.target.newMsg.value);
    if(selectedChat){
      //console.log(selectedChat);
      if (groupList.indexOf(selectedChat) > -1) {
          //Is a group. 
          //console.log("New Group Msg");
          const newMsg = ({group_name:selectedChat , 
                          from:currentUser.userid, 
                          body:newMsgBody,
                          date:Date()
                         });
          if(messageList.length === 0){
            setMessageList([newMsg]);
          }else{
            setMessageList([...messageList,newMsg]);
          }
          try{
            const response = await postGroupMsg(newMsg);
            //console.log(response); //This is message success.
          }catch(err){
            console.log(err);
            setErrorMessage("Server Error. Kindly contact administrator.");
          }
      } else {
          //Is a private chat.
          //console.log("New Private Msg");
          const newMsg = ({to:selectedChat , 
                          from:currentUser.userid, 
                          body:newMsgBody,
                          date:Date()
                         });
          if(messageList.length === 0){
            setMessageList([newMsg]);
          }else{
            setMessageList([...messageList,newMsg]);
          }
          try{
            const response = await postPrivateMsg(newMsg);
            //console.log(response); //This is message success.
          }catch(err){
            console.log(err);
            setErrorMessage("Server Error. Kindly contact administrator.");
          }
      }
    }else{
      //Show Error.
      console.log('No chat selected.');
      setErrorMessage("Kindly select a chat to send a message.");
    }
    e.target.newMsg.value='';
  };

  return (
    <Grid item  >
      <form onSubmit={handleSendNewMsg} width="100%">
        <Grid container  width="100%" alignItems="flex-end">
          <Grid item xs={11}>
            <TextField id="message" name="newMsg" label="Message" variant="outlined" margin="dense" fullWidth/>
          </Grid>
          <Grid item xs={1}>
            <IconButton type="submit">
              <SendIcon/>
            </IconButton>
          </Grid>
        </Grid>
      </form>
      {errorMessage && (
        <p> {errorMessage} </p>
      )}
    </Grid>
  );
}

export default SendMessageForm

