const express = require('express');
const router = express.Router();
const { isAuthenticated, checkAuthLevel } = require('../middleware/auth');

// GET /configuration
router.get('/configuration', isAuthenticated, checkAuthLevel(2), (req, res) => {
    res.render('configuration', { displayuser: req.user.username });
});

module.exports = router;
