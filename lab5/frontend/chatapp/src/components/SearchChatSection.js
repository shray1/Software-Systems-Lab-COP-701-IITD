import React from 'react';
//import PropTypes from 'prop-types';
//import { useState } from 'react';
import Grid from "@material-ui/core/Grid";
import Divider from "@material-ui/core/Divider";
import TextField from "@material-ui/core/TextField";
import "./chat.css"

const SearchChatSection = ( {searchResultList,searchForUser}) => {
    return (
    <>
    <Divider />
    <Grid item xs={12} style={{padding: '10px'}}>
        <TextField id="outlined-basic-search" label="Search" variant="outlined" fullWidth />
    </Grid>
    <Divider />
    </>
  )
}


export default SearchChatSection
