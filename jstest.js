
const mucc = require('./BUILD/mucc.js')();
const FS = mucc.FS;

const conv = mucc.cwrap('conv', null, ['string', 'string']);

const mml = 'A t180 @0 V127 D0 Y0,0,0,0 E255,115,0,0 l16 q0 o5 b>e8.&e2 r4';
FS.writeFile('/source.pmml', mml);

conv('/source.pmml', '/output.pmd');

let outFile = FS.readFile('/output.pmd', {encoding: 'binary'});
const fs = require('fs');

fs.writeFile('output.pmd', outFile, function(err) {
    if(err)
        console.error(err.message);
});
