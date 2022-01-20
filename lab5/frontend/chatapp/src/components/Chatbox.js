import React from 'react';
import PropTypes from 'prop-types';
import { useState } from 'react';
import ChatMsgsListBox from './ChatMsgsListBox';
import SendMessageForm from './SendMessageForm';
import Grid from "@material-ui/core/Grid";

import "./chat.css";


const ChatBox = ({messageList, currentUser}) => {
  return (
    <Grid item xs={12} >
        <Grid container className="messageContainer" display="flex">  
      			<ChatMsgsListBox  messageList={messageList} currentUser={currentUser} />
      			<SendMessageForm  />
    	  </Grid>
    </Grid>
  )
}

    
export default ChatBox
