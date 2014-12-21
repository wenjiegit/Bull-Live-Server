Bull-Live-Server
================
Bull Live Server 简称 BLS ，旨在使用C++语言提供强大功能和高性能的流媒体直播服务器。

为何要写 BLS ？

1、simple rtmp server  https://github.com/winlinvip/simple-rtmp-server<br/>
  SRS很简洁却功能太少，BLS 使用C++语言强有力的现代特性，完全重新架构和编码，
  结合基于state thread library的基础库和ST架构可以功能更强大，可以让编码更简洁，功能更强大。
<br/>
<br/>
2、crtmp server  http://www.rtmpd.com/<br/>
  CRTMPD功能很完善，架构上却很复杂，C++代码也写得晦涩难懂。<br/>
  而BLS会使用更优秀的C++特性。<br/>
<br/>
3、nginx-rtmp  https://github.com/arut/nginx-rtmp-module<br/>
  这个基于nginx，性能自不必说，功能比crtmp少，但是纯异步架构的复杂度和简洁性都不够好。<br/>
<br/>
4、red 5 http://www.red5.org/<br/>
  red 5 性能有瓶颈，而且和新的一些rtmp 服务器对接会有问题。<br/>
<br/>
5、wowza http://www.wowza.com/<br/>
  基于java语言，收费版，不适合屌丝们部署，但架构比较陈旧，而且配置很繁琐，正在作古之中。<br/>
<br/>
6、FMS http://www.adobe.com<br/>
  现在叫AMS，功能和性能都不差，基本什么功能都有，毕竟是adobe自家的，但是license很昂贵！<br/>
<br/>
  <br/>
比较了这么多，BLS要做什么？目标是什么？定位是什么?<br/>
BLS 就是为了汲取上述软件的特点，做一个更好用的直播服务器，要免费，要开源，功能和性能不打折，<br/>
代码要简洁、易懂。<br/>

usage
=====
**Step 1**: get BLS

> git clone https://github.com/wenjiegit/Bull-Live-Server.git&& cd
> Bull-Live-Server/trunk

**Step 2**: build BLS

> ./autogen.sh && make

**Step 3**: start BLS

> ./bin/bls -f conf/bls.conf

Plan
================
[1]  support AMF0/AMF3<br/>
[2]  support RTMP streaming............................................**OK**<br/>
[3]  support HLS<br/>
[4]  support HDS<br/>
[5]  support HTTP FLV streaming<br/>
[6]  support transcode with ffmpeg<br/>
[7]  support multi_process...............................................**OK**<br/>
[8]  support nginx like configure file...................................**OK**<br/>
[9]  support configure reload<br/>
[10] support fast gop.......................................................**.OK**<br/>
[11] support dvr(digital video record)<br/>
[12] support fast hls.<br/>

Code Line History
================
[0.1]&nbsp;&nbsp;2014-09-19 &nbsp;&nbsp;&nbsp; line 10901<br/>
[0.1]&nbsp;&nbsp;2014-12-21 &nbsp;&nbsp;&nbsp; line 13394, refine code structure.<br/>

history
================
[0.1]&nbsp;&nbsp;2014-12-20&nbsp;&nbsp;&nbsp;multi-process push supported.<br/>
[0.1]&nbsp;&nbsp;2014-10-10&nbsp;&nbsp;&nbsp;support multiprocess.<br/>
[0.1]&nbsp;&nbsp;2014-09-19&nbsp;&nbsp;&nbsp;support fmle or flash pushlish stream.<br/>
[0.1]&nbsp;&nbsp;2014-09-19&nbsp;&nbsp;&nbsp;support flash or vlc like play stream.<br/>
[0.1]&nbsp;&nbsp;2014-09-19&nbsp;&nbsp;&nbsp;support amf0 serialize.<br/>
[0.1]&nbsp;&nbsp;2014-07-05&nbsp;&nbsp;&nbsp;init project.<br/>
