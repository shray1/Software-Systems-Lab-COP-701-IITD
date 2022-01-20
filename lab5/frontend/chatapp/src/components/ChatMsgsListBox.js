import React from 'react';
//import PropTypes from 'prop-types';
import { useEffect, useRef } from 'react';
import Grid from "@material-ui/core/Grid";
import List from "@material-ui/core/List";
import ListItem from "@material-ui/core/ListItem";
import ListItemText from "@material-ui/core/ListItemText";
//import TextField from "@material-ui/core/TextField";
//import IconButton from "@material-ui/core/IconButton";
//import SendIcon from "@material-ui/icons/Send";
import "./chat.css";
import Paper from "@material-ui/core/Paper"; 


const ChatMsgsListBox = ({messageList,currentUser}) => {
   useEffect(() => {
    scrollToBottom()
  }, [messageList]);

   
  const bottomDivInChatEndRef = useRef(null)
  const scrollToBottom = () => {
    //bottomDivInChatEndRef.current?.scrollIntoView({ behavior: "smooth" })
    if(bottomDivInChatEndRef.current){
        bottomDivInChatEndRef.current.scrollIntoView({ behavior: "smooth" })
    }
  } 
  return (
      <Grid item style={{overflow:"auto"}}>
            {messageList.length>0 ? 
              (
                <Paper style={{maxHeight: 580, overflow: 'auto'}}>
                <List>
                  {messageList.map((m) => (
                    <ListItem key={m.id} alignItems="flex-start">
                        <Grid container>
                            <Grid item xs={12}>
                                <ListItemText align={currentUser.id===m.from?("right"):("left")} primary={m.body}>
                                </ListItemText>
                            </Grid>
                            <Grid item xs={12}>
                                <ListItemText align={currentUser.id===m.from?("right"):("left")} secondary={m.date}>
                                </ListItemText>
                            </Grid>
                        </Grid>
                    </ListItem>
                  ))}
                  <div ref={bottomDivInChatEndRef} />
                </List>
                </Paper>

              )
              :
              (<p>Enter a message to start a conversation! </p>)
            }
        
    </Grid>
  )
}

export default ChatMsgsListBox
