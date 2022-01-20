import React from 'react';
import PropTypes from 'prop-types';
import { useState } from 'react';
import "./login.css"
//import Button from "@material-ui/core/Button"

const loginUser = async (credentials) => {
  const response = await fetch('http://localhost:5000/login', {
     method: 'POST',
     headers: {
       'Content-Type': 'application/json'
     },
     body: JSON.stringify(credentials)
   });
  //console.log(response);
  return response;
}


const Login = ({setToken , setCurrentUser}) => {
  const [username, setUserName] = useState();
  const [password, setPassword] = useState();
  const [errorMessage, setErrorMessage] = useState('');
  const handleSubmit = async (e) => {
    e.preventDefault();
    //console.log("Submit Was Pressed");
    var username = (e.target.username.value);
    var password = (e.target.password.value);
    var credentials = {userid : username , password:password};
    //console.log(credentials);
    try{
      const response = await loginUser(credentials);
      const responseBody = await response.json();
      if(response.status === 200){
        setCurrentUser({userid:responseBody.id,
                      id:responseBody.id,
                      name:responseBody.name,
                      token:responseBody.token,
                      privatechats:responseBody.privatechats,
                      groupchats:responseBody.groupchats
                      });
        setToken(true);
      }else{
        console.log("Login Failure");
        //console.log(responseBody);
        const str = 'Login Error : '.concat(responseBody.loginError).concat('...Please try again.');
        setErrorMessage(str);
      }
      
    }catch(err){
      console.log(err);
      setErrorMessage("Server Error. Kindly contact administrator.");
    }
    e.target.username.value = ''
    e.target.password.value = ''
  }

  return(
    <div className="login-wrapper">
      <h1>Please Log In</h1>
      <form onSubmit={handleSubmit}>
        <label>
          <p>Username</p>
          <input type="text" name="username" />
        </label>
        <label>
          <p>Password</p>
          <input type="password" name="password" />
        </label>
        <div>
          <button type="submit">Submit</button>
        </div>
      </form>
      {errorMessage && (
        <p> {errorMessage} </p>
      )}
    </div>
  )
}

Login.propTypes = {
  setToken: PropTypes.func.isRequired
}

export default Login

