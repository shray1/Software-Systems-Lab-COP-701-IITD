const mongoose = require('mongoose');
const Schema = mongoose.Schema;

// Create Schema for Users
const GroupSchema = new Schema({
    name: {
        type: String,
        required: true,
    },
    members: {
        type: Array,
        default: [],
    },
});

module.exports = Groups = mongoose.model('groups', GroupSchema); 
