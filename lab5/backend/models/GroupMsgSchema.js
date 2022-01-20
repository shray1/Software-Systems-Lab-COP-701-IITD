const mongoose = require('mongoose');
const Schema = mongoose.Schema;

// Create Schema for Users
const GroupMsgSchema = new Schema({
    from: {
        type: String,
        required: true,
    },
    group_name: {
        type: String,
    },
    date : {
        type: String,
        required: true,
    },
    body: {
        type: String,
        required: true,
    },
});

module.exports = GroupMsgs = mongoose.model('groupmsgs', GroupMsgSchema);
