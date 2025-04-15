
var yang_mouse_keyboard_enabled=true;


function initScreenPlayer(){
	 yang_mouse_keyboard_enabled=true;
}

function initEvent(){
	    ///// mouse event
    rtc_media_player.addEventListener("mousedown", MouseDown, false);
    rtc_media_player.addEventListener("mouseup", MouseUp, false);
    rtc_media_player.addEventListener("dblclick", MouseDDown, false);
   // rtc_media_player.addEventListener("mousemove", MouseMove, false);
    rtc_media_player.addEventListener("mousewheel", MouseWheel, false);
    rtc_media_player.addEventListener("DOMMouseScroll", MouseWheel, false);
    rtc_media_player.addEventListener("contextmenu", function (e) { e.preventDefault(); return false; }, false);
    
    /// key event
    rtc_media_player.setAttribute("tabindex", "1"); //设置焦点，这样才能获得键盘输入
    rtc_media_player.focus();
    rtc_media_player.addEventListener("keydown", KeyDown, false);
    rtc_media_player.addEventListener("keyup", KeyUp, false);
}
////[0,65535]
function getPos(event)
{
    event = event || window.event;
    // firefox , event.pageX = undefined
    var scrollLeft = document.documentElement.scrollLeft || document.body.scrollLeft || 0;
    var scrollTop = document.documentElement.scrollTop || document.body.scrollTop || 0;

    var x = event.pageX != undefined ? event.pageX : event.clientX;
    var y = event.pageY != undefined ? event.pageY : event.clientY;

    // var x = event.x;
    // var y = event.y;
    var borderW, borderH;
    var rect = rtc_media_player.getBoundingClientRect();

    var W = rtc_media_player.clientWidth;
    var H = rtc_media_player.clientHeight;

    borderW = ((rect.bottom - rect.top) - H) / 2;
    borderH = ((rect.right - rect.left) - W) / 2;

    x = x - rect.left - borderW - scrollLeft;
    y = y - rect.top - borderH - scrollTop;

    if (x < 0) {
        x = 0;
    }
    if (y < 0) {
        y = 0;
    }
    if (x >= W) {
        x = W - 1;
    }
    if (y >= H) {
        y = H - 1;
    }

    ///取整
    x = x/rtc_media_player.clientWidth;//parseInt(x*65536/rtc_media_player.clientWidth);
    y = y/rtc_media_player.clientHeight;//parseInt(y*65536/rtc_media_player.clientHeight);
    //////
    return {
        x: x,
        y: y
    }
}

var left, right;
left = 0; // isIE() ? 1 : 0;
right = 2;
function Mouse(type, button, e)
{
    //
    if (!yang_mouse_keyboard_enabled) return; ///

    ////
    var pt = getPos(e);
    var direction;
    if (type === 1) { // down
        if (button == right) {
			direction="right";
		}else {
			direction="left";
		 ///left down
		 }
        
         sendevent("mousePress",direction,pt.x,pt.y);
    }
    else if (type === 2) {//up
        //if ( button == right) flags = 0x10; // right up
        //else flags = 0x04; ///left up
       if (button == right) {
			direction="right";
		}else {
			direction="left";
		 ///left down
		 }
		 sendevent("mouseRelease",direction,pt.x,pt.y);
    }
    else if (type === 3) {//move
        //flags = 0x01; /// move
        return;
        ///
    }
    /////
   // var hdr = new Uint8Array(18); ///
   // hdr[0] = 4; /// CMD_MOUSE
   // hdr[1] = 0;

   // htonl(hdr, 2, parseInt(flags)); // flags
   // hdr[6] = hdr[7] = hdr[8] = hdr[9] = 0; //delta

   // htonl(hdr, 10, parseInt(pt.x));
   // htonl(hdr, 14, parseInt(pt.y));

    /////
   // wsock_send(hdr);
    //////
 //   console.log("Mouse Event: "+ pt.x +" "+pt.y);
}


function MouseDDown(e)
{
    ///
     e.preventDefault(); 
    rtc_media_player.setAttribute("tabindex", "1"); //设置焦点，这样才能获得键盘输入
    rtc_media_player.focus();
  if (!yang_mouse_keyboard_enabled) return false; ///

    ////
    var pt = getPos(e);
      //if (button == right) {
		//	direction="right";
		//}else {
			direction="left";
		 ///left down
		// }
        
         sendevent("mouseDouble",direction,pt.x,pt.y);
         return false;
}
function MouseDown(e)
{
    ///
    rtc_media_player.setAttribute("tabindex", "1"); //设置焦点，这样才能获得键盘输入
    rtc_media_player.focus();

   // console.log("MouseDown: "  );
    Mouse(1, e.button, e);
}
function MouseUp(e)
{
    Mouse(2, e.button, e);
}

function MouseMove(e)
{
    Mouse(3, 0,  e);
}

function MouseWheel(e )
{
    if (!yang_mouse_keyboard_enabled) return;
    ////

    var pt = getPos(e);
    var delta = Math.max(-1, Math.min(1, (e.wheelDelta || -e.detail)))*100;
	sendevent("wheel",delta,pt.x,pt.y);
  //  var hdr = new Uint8Array(18); ///
   // hdr[0] = 4; /// CMD_MOUSE
  //  hdr[1] = 0;
  //  var flags = 0x0800; // MOUSEEVENTF_WHEEL

   // htonl(hdr, 2, parseInt(flags)); // flags
  //  htonl(hdr, 6, parseInt(delta ));

   // htonl(hdr, 10, parseInt(pt.x));
   // htonl(hdr, 14, parseInt(pt.y));

   // wsock_send(hdr);
    ////
  //  console.log('mousewheel');
  	//var obj={"x":"0","y":"0","direction":""+delta,"evt":"double"};
	//console.log("send data:"+obj);
	// ws.send(obj); //将消息发送到服务端
}

////////
function Key(type, e)
{
    if (!yang_mouse_keyboard_enabled) return;

    ////
  //  var flags = 0; /// key down
    //if (type == 2) flags = 0x02; /// key up
    
    if(type==1)
		sendevent("keyPressEvent","key",0,e.keyCode);
    else
		sendevent("keyReleaseEvent","key",0,e.keyCode);
    ////

    //var hdr = new Uint8Array(18);
   // hdr[0] = 5; // CMD_KEYBOARD
   // hdr[1] = 0;
   // htonl(hdr, 2, parseInt(flags)); // flags

    //htons(hdr, 6, parseInt(e.keyCode)); // virtual code
   // htons(hdr, 8, 0); // scan code
	//var obj={"x":"0","y":"0","direction":e.keyCode,"evt":"double"};
	//console.log("send data:"+obj);
	// ws.send(obj); //将消息发送到服务端
    ////
   // wsock_send(hdr); ///
}
function KeyDown(e)
{
    // alert("key down: " + e.keyCode );
    Key(1, e);
}

function KeyUp(e)
{
    Key(2, e);
}

