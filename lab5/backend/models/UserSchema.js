const mongoose = require('mongoose');
const Schema = mongoose.Schema;

// Create Schema for Users
const UserSchema = new Schema({
    userid: {
        type: String,
        required: true,
    },
    password: {
        type: String,
        required: true,
    },
    name: {
        type: String,
        required: true,
    },
    privatechats: {
        type: Array,
        default: [],
    },
    groupchats: {
        type: Array,
        default: [],
    },
});
const Users = mongoose.model('users', UserSchema);
module.exports = Users
