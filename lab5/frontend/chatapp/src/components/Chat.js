import React from 'react';
//import PropTypes from 'prop-types';
import { useState , useEffect} from 'react';
import ChatList from './ChatList';
import ChatMsgsListBox from './ChatMsgsListBox';
//import Chatbox from './Chatbox';
import Grid from "@material-ui/core/Grid";
import Paper from "@material-ui/core/Paper";
import "./chat.css";
import SendMessageForm from './SendMessageForm'
import SearchChatSection from './SearchChatSection'
import LoggedUserSection from './LoggedUserSection'
import Divider from "@material-ui/core/Divider";
//import ListItemIcon from "@material-ui/core/ListItemIcon";
import Typography from "@material-ui/core/Typography";
//import socketIOClient from "socket.io-client";
import io from "socket.io-client";
//import openSocket from 'socket.io-client';


const Chat = ({currentUser,messageList,setMessageList,chatUserList,setChatUserList,
    selectedChat,setSelectedChat,searchResultList,setSearchResultList,groupList,setGroupList}) => {
    

    //const socket = openSocket('http://localhost:5000/');
    const [socket, setSocket] = useState(null);

    useEffect(() => {
        const newSocket = io('http://localhost:5000/', { transports : ['websocket'] });
        //newSocket.on('message',getChatList);
        setSocket(newSocket);
        return () => newSocket.close();

    }, [setSocket]);

    useEffect(() => {
        getChatList();
        getGroupNameList();
    },[])


    const getChatList = async()=>{
        const  chatList = await fetchChatList();
        const jointList = chatList.privateChatList.concat(chatList.groupChatList);
        setChatUserList(jointList);
    }
    const getGroupNameList = async()=>{
        const groupNameList = await fetchGroupList();
        const groupListt = groupNameList.groupNames;
        //console.log("Group List is ");
        //console.log(groupListt);
        setGroupList(groupListt);
    }
    const fetchChatList = async () => {
        const response = await fetch('http://localhost:5000/chatlist/', {
                             method: 'POST',
                             headers: {
                               'Content-Type': 'application/json',
                               'Authorization': "Bearer " + currentUser.token 
                             },
                             body: JSON.stringify(({userid:currentUser.id}))
                           });
        return response.json();
    };
    const fetchGroupList = async () => {
        const response = await fetch('http://localhost:5000/allgroupnames/', {
                             method: 'GET',
                             headers: {
                               'Content-Type': 'application/json',
                               'Authorization': "Bearer " + currentUser.token 
                             }
                           });
        return response.json();
    };

      return (
        <div>
            <Grid >
                <Grid item xs={12}  >
                    <Typography variant="h5" className="header-message" align="center">Lab5 Chat-App</Typography>
                </Grid>
            </Grid>
            <Grid container justifyContent="center"  component={Paper}  style={{width: '100%',height: '80vh'}}>
                <Grid item xs={3} style={{borderRight: '2px solid #e0e0e0' , overflow:"auto"}}>
                    <LoggedUserSection currentUser={currentUser}/>
                    <SearchChatSection currentUser={currentUser} searchResultList={searchResultList} setSearchResultList={setSearchResultList}/>
                    <ChatList chatUserList={chatUserList} setSelectedChat={setSelectedChat} 
                          setMessageList={setMessageList} currentUser={currentUser} groupList={groupList} socket={socket}/>
                </Grid>
                <Grid item xs={9} style={{overflow:"auto"}}>
                    <ChatMsgsListBox selectedChat={selectedChat} messageList={messageList} currentUser={currentUser}/>
                    <Divider />
                    <SendMessageForm selectedChat={selectedChat} currentUser={currentUser} setMessageList={setMessageList} messageList={messageList} groupList={groupList}/>
                </Grid>
            </Grid>
            </div>
      );
}

export default Chat

