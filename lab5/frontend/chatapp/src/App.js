import React from 'react';
import { useState } from 'react'
//import { BrowserRouter as Router,Routes,Route } from 'react-router-dom'
import Chat from './components/Chat'
import Login from './components/Login'


function App() {
  const [token, setToken] = useState(false);
  const [chatUserList, setChatUserList] = useState([]);
  const [currentUser,setCurrentUser] = useState();
  const [messageList,setMessageList] = useState([]) ; 
  const [selectedChat,setSelectedChat] = useState();
  const [searchResultList,setSearchResultList] = useState([]);
  const [groupList,setGroupList] = useState([]);
  if(!token) {
    return (<Login setToken={setToken} setCurrentUser={setCurrentUser} />);
  }
  return (<Chat chatUserList={chatUserList} setChatUserList={setChatUserList} 
                messageList={messageList} setMessageList={setMessageList}
                currentUser={currentUser}
                selectedChat={selectedChat} setSelectedChat={setSelectedChat}
                searchResultList={searchResultList} setSearchResultList={setSearchResultList}
                groupList={groupList}  setGroupList={setGroupList}
          />);
}
export default App;

