import React from 'react';
//import PropTypes from 'prop-types';
//import { useState ,useEffect} from 'react';
import "./chat.css"
import ListItem from "@material-ui/core/ListItem";
import ListItemIcon from "@material-ui/core/ListItemIcon";
import ListItemText from "@material-ui/core/ListItemText";
import Avatar from "@material-ui/core/Avatar";
//import socketIOClient from "socket.io-client";

const SingleChatListItem = ({currentUser,chatUser,setSelectedChat,setMessageList,groupList,socket}) => {
    
    const handleClickChatItem = async() => {
        if (groupList.indexOf(chatUser) > -1) {
            //Is a group. 
            setSelectedChat(chatUser);
            const resultList= await getGroupMsgsList();
            //console.log(resultList);
            setMessageList(resultList);
        } else {
            //Is a private chat. 
            setSelectedChat(chatUser);
            const resultList= await getPrivateMsgsList();
            //console.log(resultList);
            setMessageList(resultList);
        }
    }

    const updatePrivateMessageList = async () => {
      const resultList = await getPrivateMsgsList();
      setMessageList(resultList);
    }
    const updateGroupMessageList = async () => {
      const resultList = await getGroupMsgsList();
      setMessageList(resultList);
    }
    
    
    const updateChatHandle = (data) => {
      //console.log("Update Chat Socket Event.");
      if( data.type === 'private'){
        //console.log(data);
        if(chatUser === data.from){
          updatePrivateMessageList();
        }
      }else{
        if(chatUser === data.group_name){
          updateGroupMessageList();
        }
      }
    }

    socket.on('updatechat',updateChatHandle);

    const getPrivateMsgsList = async()=>{
        const  msgList = await fetchPrivateMsgList();
        //console.log(msgList);
        return msgList.privateMsgList;
        //setMessageList(msgList.privateMsgList);
    }
    const getGroupMsgsList = async()=>{
        const  msgList = await fetchGroupMsgList();
        return msgList.groupMsgList;
        //setMessageList(msgList.groupMsgList);
    }
    const fetchPrivateMsgList = async() => {
      //console.log(({from:currentUser.id,to:chatUser}));
      const response = await fetch('http://localhost:5000/privatemsgs/', {
                             method: 'POST',
                             headers: {
                               'Content-Type': 'application/json',
                               'Authorization': "Bearer " + currentUser.token 
                             },
                             body: JSON.stringify(({from:currentUser.id,to:chatUser}))
                           });
        return response.json();
    }
    const fetchGroupMsgList = async() => {
      const response = await fetch('http://localhost:5000/groupmsgs/', {
                             method: 'POST',
                             headers: {
                               'Content-Type': 'application/json',
                               'Authorization': "Bearer " + currentUser.token 
                             },
                             body: JSON.stringify(({group_name:chatUser}))
                           });
        return response.json();
    }
    return (
      <ListItem button key={chatUser} alignItems="flex-start"  onClick={handleClickChatItem}>
          <ListItemIcon>
              <Avatar alt={chatUser} src="https://material-ui.com/static/images/avatar/1.jpg" />
          </ListItemIcon>
          <ListItemText primary={chatUser.name}>{chatUser}</ListItemText>
      </ListItem>
  )
}


export default SingleChatListItem
