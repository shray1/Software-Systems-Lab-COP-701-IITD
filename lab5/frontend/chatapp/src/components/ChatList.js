import React from 'react';
//import PropTypes from 'prop-types';
//import { useState } from 'react';
import SingleChatListItem from './SingleChatListItem';
import List from "@material-ui/core/List"; 
//import Grid from "@material-ui/core/Grid";
import Paper from "@material-ui/core/Paper"; 
import "./chat.css"


const ChatList = ( {chatUserList, setSelectedChat, setMessageList,currentUser,groupList,socket}) => {
    return (
      <Paper style={{maxHeight: 480, overflow: 'auto'}}>
        <List>
          {chatUserList.map((chatUser) => (
            <SingleChatListItem chatUser={chatUser} setSelectedChat={setSelectedChat} 
                setMessageList={setMessageList} groupList={groupList} currentUser={currentUser}
                  socket={socket} />
          ))}
        </List>
      </Paper>
  )
}


export default ChatList
