const mongoose = require('mongoose');
const Schema = mongoose.Schema;

// Create Schema for Users
const PrivateMsgSchema = new Schema({
    from: {
        type: String,
        required: true,
    },
    to: {
        type: String,
        required:true,
    },
    date : {
        type: String,
        required: true,
    },
    body: {
        type: String,
    },

});

module.exports = PrivateMsgs = mongoose.model('privatemsgs', PrivateMsgSchema);
