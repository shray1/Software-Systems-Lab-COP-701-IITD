import React from 'react';
//import PropTypes from 'prop-types';
//import { useState } from 'react';
import List from "@material-ui/core/List";
import ListItem from "@material-ui/core/ListItem";
import ListItemText from "@material-ui/core/ListItemText";
//import TextField from "@material-ui/core/TextField";
//import IconButton from "@material-ui/core/IconButton";
import Avatar from "@material-ui/core/Avatar";
import "./chat.css"
import ListItemIcon from "@material-ui/core/ListItemIcon";


const LoggedUserSection = ( {currentUser}) => {
    return (
    <List>
        <ListItem button key={currentUser.id}>
            <ListItemIcon>
            <Avatar alt={currentUser.name} src="https://material-ui.com/static/images/avatar/1.jpg" />
            </ListItemIcon>
            <ListItemText primary={currentUser.name}></ListItemText>
        </ListItem>
    </List>
  )
}


export default LoggedUserSection
