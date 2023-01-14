

// Max 2'147'483'648 = 2 ^ 31   = 2GB
function nearestHighestPowerOf2(x) {
   if (x>2147483648)
      return 4294967296;
   y = x;
   y |= y >> 1;
   y |= y >> 2;
   y |= y >> 4;
   y |= y >> 8;
   y |= y >> 16;
   y++;
   return y;
}

// Want to display 32 rows (each with 10 pixels height hence 320 pixesl)
// So 32 rows of 1024 pixels so the scale is how much each pixels
// represent in bytes
// TODO I am sure I can simplify this... (sweating smile)
function getScaleBasedonMaxValue(x) {
   x = x/32768;  // 1024*32
   if (x>=65536) //  <= 4GB
      return 131072;
   if (x>=48000) // <= 2 GB
      return 65536;
   if (x>=32768)    // <= 1.5 GB
      return 4800;  
   if (x>=16384)    // <= 1GB
      return 32768;
   if (x>=8192)    // <= 500 MB
      return 16384;
   if (x>=4096)   // <= 250MB
      return 8192;
   if (x>=2048)    // <= 100 MB
      return 4096;
   if (x>=512)    // <= 50 MB
      return 2048;
   if (x>=32)       // <= 10 MB
      return 512;
   if (x>=16)     // <= 1 MB 
      return 32;
   if (x>=8)     // <= 500 kb
      return 16;  
   if (x>=4)     // <= 256 kb
      return 8;  
   if (x>=2)     // <= 128 kb
      return 4;  
   if (x>=1)     // <= 64 kb
      return 2;  
   return 1;    // 32 kb KB
}

// Want to display 32 rows (each with 10 pixels height hence 320 pixesl)
// So 32 rows of 1024
function getScaleBasedonMaxValueOLD(x) {
   x = x/262144;
   if (x>=8192) //  <= 4GB
      return 16384;
   if (x>=6000) // <= 2 GB
      return 4096;
   if (x>=4096)    // <= 1.5 GB
      return 6000;  
   if (x>=2048)    // <= 1GB
      return 4096;
   if (x>=1024)    // <= 500 MB
      return 2048;
   if (x>=512)   // <= 250MB
      return 1024;
   if (x>=256)    // <= 100 MB
      return 512;
   if (x>=64)    // <= 50 MB
      return 256;
   if (x>=4)       // <= 10 MB
      return 64;
   if (x>=2)     // <= 1 MB 
      return 4;
   if (x>=1)     // <= 500 kb
      return 2;  
   return 1;    // 256 KB
}

function drawClearCanvas(w, h) {
   
   var canvas = document.getElementById('canvas');
   if (canvas.getContext) {
     var ctx = canvas.getContext('2d');
 
     ctx.fillStyle = '#F9DC5C';
     //context.fillRect(20,20,100,100);
     //context.clearRect(40,40,60,60);
     ctx.strokeRect(0,0,w,h);
   }
}

function drawBox(cw, x,y,w,h,color) {
   var canvas = document.getElementById(cw); //'canvas');
   if (canvas.getContext) {
      var ctx = canvas.getContext('2d');
      ctx.fillStyle = color;
      ctx.fillRect(x,y,w,h);
      ctx.strokeRect(x,y,w,h);
   }
}

/*function Heap (maxAddress, nbHeap) {
   this.maxAddress = maxAddress;
   this.nbHeap = nbHeap;
   this.ptr = 0;

   this.next = function() {
      this.ptr++;
      if (this.ptr > this.nbHeap)
         this.ptr = 0;
   }

   this.getPtr = function() {
      return 6; //this.ptr;
   }
}

heap = new Heap(valueMax, nbLines);
*/
