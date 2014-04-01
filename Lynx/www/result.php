<HTML>
<HEAD>
<TITLE> NewsIR </TITLE>
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<meta http-equiv="imagetoolbar" content="no">
<link href="js/jquery-ui-themes.css" type="text/css" rel="stylesheet">
<link href="js/axure_rp_page.css" type="text/css" rel="stylesheet">
<link href="js/axurerp_pagespecificstyles_result.css" type="text/css" rel="stylesheet">

<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<script type="text/javascript" src="js/jquery.js"></script>
<link rel="stylesheet" href="js/jquery.bigautocomplete.css" type="text/css" />
<script type="text/javascript" src="js/jquery.bigautocomplete.js"></script>

<!--[if IE 6]>
<![endif]-->
<SCRIPT src="js/jquery-1.4.2.min.js"></SCRIPT>
<SCRIPT src="js/jquery-ui-1.8.10.custom.min.js"></SCRIPT>
<SCRIPT src="js/axurerp_beforepagescript.js"></SCRIPT>
<SCRIPT src="js/messagecenter.js"></SCRIPT>
<style type="text/css">
<!--
body {
	background-color: #FFFFFF;
	background-image: url();
}
-->
	div#container1{width:auto;}
		div#menu1 {margin-left:9%;}
	div#logo_inputAndbotton{width:auto;height:6%;position:relative;background-color:}
		div#logo{margin-left:1%;float:left;}
		div#inputAndbutton{margin-left:9%;}
		
	div#container_newscontent{width:50%;float:left;}
		div#divNews0{margin-left:18%;}
		div#divNews1{margin-left:18%;}
		div#divNews2{margin-left:18%;}
		div#divNews3{margin-left:18%;}
		div#divNews4{margin-left:18%;}
		div#divNews5{margin-left:18%;}
		div#divNews6{margin-left:18%;}
		div#divNews6{margin-left:18%;}
		div#divNews7{margin-left:18%;}
		div#divNews8{margin-left:18%;}
		div#divNews9{margin-left:18%;}
	div#container_peopleAlsoSearch{width:44%;float:left;}
		div#peopleAlsoSearch_head{width:75%;float:left;margin-left:25%;}
		div#container_peopleAlsoSearch1{float:left;margin-left:25%;}
		div#container_peopleAlsoSearch2{float:left;margin-left:5%;}
	div#txt_image_container{font-size:13px;color:#000;width:40%;float:left;left:54%;top:6%;_right:6%;visibility:hidden;border:2px solid #;background:#F5F5F5;overflow:hidden;position:fixed;_position:absolute;}
		div#txt_image_container img{float:left;margin:3.125% 3.125% 1.05% 1.05%;}
		<!--
			div#txt{width:480px;float:left;margin-left:80px;margin-top:50px;}
		-->
	div#container_searchRec{width:44%;float:left;}
	div#container_divPagenation{width:80%;float:left;}
		div#divPagenation{width:44%;margin-left:11.25%;}
.STYLE4 {font-family: Arial}
	#firstNewsContainer{}
	#firstNewsContainer img{float:left;margin:0px 10px 8px 0px;}
</style>   
</HEAD>
<BODY style="position:relative;">

<?php 
	if($_POST){
	if(! $_POST["page"]){ $page = '1'; }
	else{$page = $_POST["page"];}
	$tokens = $_POST['tokens'];
	$model = $_POST['model'];
	}
	
	if($_GET){
	$page = $_GET["page"];
	$tokens = $_GET['tokens'];
	$model = $_GET['model'];
	}

	//去除多余字符，保留数字
	$tokensRegVs = '/[^a-zA-Z\s]*/';
	$tokensRegBo = '/[^a-zA-Z\(\)\-\|\&]*/';
	$tokensRegKp = '/[^a-zA-Z0-9\s]*/';
	if($model == "vector"){
		$tokens = preg_replace($tokensRegVs,'',$tokens);
	}
	else if($model == "boolTime" || $model=="boolPop"){
		$tokens = preg_replace($tokensRegBo,'',$tokens);
	}
	else if($model == "kProxTime" || $model == "kProxPop"){
		$tokens = preg_replace($tokensRegKp,'',$tokens);
	}
	//echo "<#>".trim($tokens)."<#>";
	//限制搜索次数
	/*$tokensArr = explode(" ",$tokens);
	while(count($tokensArr) > 30)
	{
		array_push($tokensArr);
	}
	$tokens = "";
	for($i=0;$i<count($tokensArr);$i++)
	{
		$tokens .= $tokensArr[$i] . " ";
	}*/
	
	/*socket news*/
	// Create the socket and connect 
	$socketNews = socket_create(AF_INET, SOCK_STREAM, SOL_TCP) or die("news socket create failed.."); 
	$connection = socket_connect($socketNews,'127.0.0.1', 7861) or die("news socket connect failed.."); 

	//写数据到socket缓存   json_encode($_POST) 		
	//echo $tokens;
	socket_write($socketNews, $page . '<#>' . trim($tokens) . '<#>' . $model) or die("news socket write failed..");
	while($temp = socket_read($socketNews, 1024, PHP_BINARY_READ )) {
		//cho $temp . "<br /><br />";
		$newsBuffer .= $temp;
	}	
	socket_shutdown($socketNews, 2) or die("news socket close failed..");
	//echo $newsBuffer;
	
	//socket related search
	// Create the socket and connect 
	$socketRelate = socket_create(AF_INET, SOCK_STREAM, SOL_TCP) or die("related search socket create failed.."); 
	$connection = socket_connect($socketRelate,'192.168.1.120', 7862) or die("related search socket connect failed.."); 

	//写数据到socket缓存   json_encode($_POST) 
	socket_write($socketRelate, trim($tokens)) or die("related search socket write failed..");
	while($temp = socket_read($socketRelate, 1024, PHP_BINARY_READ)){ 
		$relatedBuffer .= $temp;
	}	
	socket_close($socketRelate);
	//echo $relatedBuffer;
?>

<script type="text/javascript">
	$(function(){
		//var gobal;
		var url_ = "../queryCom.php";
		$("#u0").bigAutocomplete({url:url_});                             
	})
</script>

<DIV class="main_container">
	<form action="result.php" method="post">
	<div id="container1" style="background:#000; color:#222">
	   <div  id="menu1" style="font-family:Arial">
			<select name="model" size="1" >
					  <option value="vector" <?php if($model == "vector") echo 'selected="selected"';?>>vector by relevance</option>
					  <option value="boolTime" <?php if($model == "boolTime") echo 'selected="selected"';?>>boolean by time</option>
					  <option value="boolPop" <?php if($model == "boolPop") echo 'selected="selected"';?>>boolean by popularity</option>
					  <option value="kProxTime" <?php if($model == "kProxTime") echo 'selected="selected"';?>>k-proximity by time</option>
					  <option value="kProxPop" <?php if($model == "kProxPop") echo 'selected="selected"';?>>k-proximity by popularity</option>
					</select>	     
		</div>
    </div>
	<br/>
	<div id="logo_inputAndbotton">
		<div id="logo" style="width:5.04%;height:80%;margin-left:1.8%;margin-top:0.3%;">
			<a href="search.html">
				<img src="js/logo1.png" width="100%" height="90%">
			</a>
		</div>
		<div id="inputAndbutton" style="margin-left:9%;font-family:Arial;">
			<div style="height:15%;background-color:;"></div>
			<input id="u0" type=text value="<?php echo $tokens; ?>" class="u0"  name="tokens"  autocomplete="off">
			<input name="submit" type=submit class="u1" id="u1" value="search">
		</div>
	</div>
	</form>
</div>

<div id="editCorrect" style="margin-left:9%;font-family:Arial;"></div>

<DIV id="container_newscontent">
	<div id="divNewsID0" style="display:none">id</div>
	<div id="divNewsID1" style="display:none">id</div>
	<div id="divNewsID2" style="display:none">id</div>
	<div id="divNewsID3" style="display:none">id</div>
	<div id="divNewsID4" style="display:none">id</div>
	<div id="divNewsID5" style="display:none">id</div>
	<div id="divNewsID6" style="display:none">id</div>
	<div id="divNewsID7" style="display:none">id</div>
	<div id="divNewsID8" style="display:none">id</div>
	<div id="divNewsID9" style="display:none">id</div> 
	<!---
	<div id="divNewsID0">id</div>
	<div id="divNewsID1">id</div>
	<div id="divNewsID2">id</div>
	<div id="divNewsID3">id</div>
	<div id="divNewsID4">id</div>
	<div id="divNewsID5">id</div>
	<div id="divNewsID6">id</div>
	<div id="divNewsID7">id</div>
	<div id="divNewsID8">id</div>
	<div id="divNewsID9">id</div>-->
	
	<div id="divNews0" class="STYLE4" onMouseOver="mOver(this)" onMouseOut="mOut(this)"></div><br/>
	<div id="divNews1" class="STYLE4" onMouseOver="mOver(this)" onMouseOut="mOut(this)"></div>
	<div id="divNews2" class="STYLE4" onMouseOver="mOver(this)" onMouseOut="mOut(this)"></div>
	<div id="divNews3" class="STYLE4" onMouseOver="mOver(this)" onMouseOut="mOut(this)"></div>
	<div id="divNews4" class="STYLE4" onMouseOver="mOver(this)" onMouseOut="mOut(this)"></div>
	<div id="divNews5" class="STYLE4" onMouseOver="mOver(this)" onMouseOut="mOut(this)"></div>
	<div id="divNews6" class="STYLE4" onMouseOver="mOver(this)" onMouseOut="mOut(this)"></div>
	<div id="divNews7" class="STYLE4" onMouseOver="mOver(this)" onMouseOut="mOut(this)"></div>
	<div id="divNews8" class="STYLE4" onMouseOver="mOver(this)" onMouseOut="mOut(this)"></div>
	<div id="divNews9" class="STYLE4" onMouseOver="mOver(this)" onMouseOut="mOut(this)"></div>
</DIV>
<br/>
<br/>
<div id="container_peopleAlsoSearch" >
	<div class="STYLE4" id="peopleAlsoSearch_head"></div>
	<br/>
	<div id="container_peopleAlsoSearch1" >
		<br/>
		<div id="peopleAlsoSearch0" class="STYLE4" style="font-weight:bold;"></div>
		<br/>
		<div id="peopleAlsoSearch2" class="STYLE4" style="font-weight:bold;"></div>
		<br>
		<div id="peopleAlsoSearch4" class="STYLE4" style="font-weight:bold;"></div>
	</div>
	<div id="container_peopleAlsoSearch2">
		<br/>
		<div id="peopleAlsoSearch1" class="STYLE4" style="font-weight:bold;"></div>
		<br/>
		<div id="peopleAlsoSearch3" class="STYLE4" style="font-weight:bold;"></div>
		<br/>
		<div id="peopleAlsoSearch5" class="STYLE4" style="font-weight:bold;"></div>
	</div>
</div>
<br/>
<br/>
<div id="txt_image_container">
	<div id="title">
	</div>
	<img id="image" width="192px" height="144px" />
	<p id="txt" class="STYLE4"></p>
</div>
<div id="container_searchRec">
	<p>
	</P>
	<p>
	</p>
	<br/>
    <div style="margin-left:25%;">
		<table width="100%" height="30%" cellpadding="0">
			<tbody>
				<tr>
				  <th colspan="7" align="left" class="STYLE4" valign="baseline"><strong><div id="isRelated"></div></strong>
				  <div id="searchRec8"></div>
				  <div id="searchRec9"></div></th>
				</tr>
				<tr>
					<th width="40%"  align="left" valign="baseline">
						<div id="searchRec0" class="STYLE4"></div></th>
				</tr>
				<tr>
					<th width="40%"  align="left" valign="baseline">
						<div id="searchRec1" class="STYLE4"></div></th>
				</tr>
				<tr>
					<th width="40%" align="left" valign="baseline">
						<div id="searchRec2" class="STYLE4"></div></th>
				</tr>
				<tr>
					<th width="40%" align="left" valign="baseline">
						<div id="searchRec3" class="STYLE4"></div></th>
				</tr>
				<tr>
				  <th width="40%" align="left" valign="baseline"><div id="searchRec4" class="STYLE4"></div></th>
				</tr>
				<tr>
				  <th  width="40%" align="left" valign="baseline"><div id="searchRec5" class="STYLE4"></div></th>
				</tr>
			  </tr>
			</tbody>
	    </table>
    </div>
</div>
<br/><br/>
<div id="container_divPagenation">
	<div id="divPagenation" style="font-family:Arial"></div>
	<br/>
	<br/>
</div>
<SCRIPT LANGUAGE="JavaScript">

function trim(str){ //删除左右两端的空格，括号
	return str.replace(/(^[\(\)'.:",]*)|([\(\)'.:",]*$)/g, "");
}

function replaceReg(reg,str){
	str = str.toLowerCase();
	
	return str.replace(reg,function(m){return m.toUpperCase()})
}
reg = /\b(\w)|\s(\w)/g;

s="<?php echo mysql_escape_string($newsBuffer);?>";
pageSizeCount = 5;
perPageNews = 10;
currentPage = parseInt(<?php echo $page;?>);

//split news and similar words
news_sim_arr = s.split("<similar_words>");
//alert(news_sim_arr[0]);
//split correct word and news
corr_news_arr = news_sim_arr[0].split('<edit_correct>');
//alert(corr_news_arr);

if(corr_news_arr[0] && corr_news_arr[0] != " ")
{
	corrTokens = corr_news_arr[0];
	//alert(tokens);
	divEditCorrect = document.getElementById("editCorrect");
	divEditCorrect.innerHTML='Did you mean: '+'<a href="result.php?page=1&tokens='+corrTokens+'&model=<?php echo $model; ?>">'+corrTokens+'</a><p>';
} 

tokens = "<?php echo trim($tokens); ?>";

//show pagenation
divPagenation=document.getElementById("divPagenation");
strPagenation="";
if(currentPage&&currentPage!=1)
	strPagenation+='<a href="result.php?page=<?php echo (int)($page)-1; ?>&tokens='+tokens+'&model=<?php echo $model; ?>">Previous</a>&nbsp;&nbsp;';
else
	strPagenation+='<span style="color:#808080;">Previous</span>&nbsp;&nbsp;';
for(i=1;i<=pageSizeCount;i++)
{
	if(i!=currentPage)
		strPagenation+='<a href="result.php?page='+i+'&tokens='+tokens+'&model=<?php echo $model; ?>">'+i+'</a>&nbsp;&nbsp;';
	else
		strPagenation+=i+"&nbsp;&nbsp;";
}
if(currentPage&&currentPage!=pageSizeCount)
	strPagenation+='<a href="result.php?page=<?php echo (int)($page)+1; ?>&tokens='+tokens+'&model=<?php echo $model; ?>">Next</a>&nbsp;&nbsp;';
else
	strPagenation+='<span style="color:#808080;">Next</span>&nbsp;&nbsp;';
divPagenation.innerHTML=strPagenation;

//show news
newsArr = corr_news_arr[1].split("<##>");
newsArr.pop();
divNewsArr = new Array(perPageNews);
divIDArr = new Array(perPageNews);
for(i=0;i<perPageNews;i++)
{
	divNewsArr[i] = document.getElementById("divNews"+i.toString());
	divIDArr[i] = document.getElementById("divNewsID"+i.toString());
}
for(i=0;i<perPageNews;i++)
{
	newsElements = newsArr[i].split('<#>');
	docid = newsElements[0];
	title = newsElements[1];
	url = newsElements[2];
	date = newsElements[3];
	imgID = newsElements[4];
	snippet = newsElements[5];
	
	//highlight keywords
	tokens_arr = tokens.split(" ");
	//alert(tokens_arr);
	for(j=0;j<tokens_arr.length;j++)
	{
		if(!tokens_arr[j] && tokens_arr[j] == " " )
			continue;
		
		titleArr = title.split(" ");
		title = "";
		for(k=0;k<titleArr.length;k++)
		{
			if(trim(tokens_arr[j]).toLowerCase() == trim(titleArr[k]).toLowerCase())
			{
				title += '<font color="red">'+titleArr[k]+'</font> ';
			}
			else
			{
				title += titleArr[k] + " ";
			}
		}

		snippetArr = snippet.split(" ");
		snippet = "";
		for(k=0;k<snippetArr.length;k++)
		{
			if(trim(tokens_arr[j]).toLowerCase() == trim(snippetArr[k]).toLowerCase())
			{
				snippet += '<font color="red">'+snippetArr[k]+'</font> ';
			}
			else
			{
				snippet += snippetArr[k] + " ";
			}
		}
	}
	
	if(i==0){
		tempstr = '';
		
		//alert(imgID);
		if(imgID != "0"){
			tempstr += '<a href=" ' + url + '" target="_blank"> ' + title + '</a>';
			
			tempstr += '<div  id="firstNewsContainer"><img src="img/'+imgID+'.jpg" width="128px" height="96px"/>';
			
			tempstr += '<span style="color:#808080;">'+date+'&nbsp;-'+'</span>' + '&nbsp;&nbsp;';
			tempstr += snippet + '<p></div>';
		}
		else{
			tempstr += '<a href=" ' + url + '" target="_blank"> ' + title + '</a><br />';
			
			tempstr += '<span style="color:#808080;">'+date+'&nbsp;-'+'</span>' + '&nbsp;&nbsp;';
			tempstr += snippet;
		}
	}
	else {
		tempstr = '<p>';
		tempstr += '<a href=" ' + url + '" target="_blank"> ' + title + '</a><br />';
		tempstr += '<span style="color:#808080;">'+date+'&nbsp;-'+'</span>' + '&nbsp;&nbsp;';
		tempstr += snippet + '<p>';
	}
	
	divIDArr[i].innerHTML = docid + " " + imgID;
	divNewsArr[i].innerHTML = tempstr;
}

//show similar words
//alert(news_sim_arr[1]);
simWordArr = news_sim_arr[1].split("<#>");
simWordArr.pop();
divSimWordArr = new Array(6);
if(simWordArr.length != 0)
{
	document.getElementById('peopleAlsoSearch_head').innerHTML = '<strong>People also search for:</strong>';
}
for(i=0;i<6;i++)
{
	divSimWordArr[i] = document.getElementById("peopleAlsoSearch"+i.toString());
}
for(i=0;i<simWordArr.length;i++)
{
	divSimWordArr[i].innerHTML = '<a href="result.php?page=1&tokens='+simWordArr[i]+'&model=<?php echo $model; ?>">'+simWordArr[i]+'</a>';
}

//show related search
relatedSearch = "<?php echo $relatedBuffer;?>";

relatedArr = relatedSearch.split("<#>");
relatedArr.pop();
if(relatedArr.length > 1)
{
	document.getElementById("isRelated").innerHTML = "Related search:";
	divRelatedArr = new Array(6);
	for(i=0;i<divRelatedArr.length;i++)
	{
		divRelatedArr[i] = document.getElementById("searchRec"+i.toString());
	}
	for(i=0;i<relatedArr.length-1;i++)
	{
		divRelatedArr[i].innerHTML = '<a href="result.php?page=1&tokens='+relatedArr[i+1]+'&model=<?php echo $model; ?>">'+relatedArr[i+1]+'</a>';
	}
}


</SCRIPT>

<script id="preview">
	function mOut(obj){
		var y=document.getElementById("txt_image_container");
		y.style.visibility='hidden';
	}
	function mOver(obj){
	
		var x=document.getElementById("txt_image_container");
		x.style.visibility='visible';
		//x.style.marginTop="30px";
		
		//request news
		t = obj.id;
		t = t.substr(t.length-1,1);
		id_img = document.getElementById("divNewsID"+t).innerText;
		var arr = id_img.split(" ");
		id = arr[0];
		isImg = arr[1];
		//alert(id + img);
		//alert(id);
		$.post(
			"newsPreview.php",
			{keyword:id},
			function(result){
				result = $.parseJSON(result);
				var img = result.img;
				if(isImg != 0){
					document.getElementById("image").src = img;
				}
				else{
					document.getElementById("image").src = "\img\\"+isImg+".jpg";
				}
				document.getElementById("title").innerHTML = '<p align="center" style="font-size:15px" class="STYLE4"><b>'+result.title+'</b></p>';
				document.getElementById("txt").innerHTML = result.content;
			}
		);
		//var y=document.getElementById("txt");
		//y.innerHTML="Stewart and his team put out several issues of The Whole Earth Catalog, and then when it had run its course, they put out a final issue. It was the mid-1970s, and I was your age. On the back cover of their final issue was a photograph of an early morning country road, the kind you might find yourself hitchhiking on if you were so adventurous. Beneath it were the words: \"Stay Hungry. Stay Foolish.\"It was their farewell message as they signed off. Stay Hungry. Stay Foolish. And I have always wished that for myself. And now, as you graduate to begin anew, I wish that for you.Stewart and his team put out several issues of The Whole Earth Catalog, and then when it had run its course, they put out a final issue. It was the mid-1970s, and I was your age. On the back cover of their final issue was a photograph of an early morning country road, the kind you might find yourself hitchhiking on if you were so adventurous. Beneath it were the words: \"Stay Hungry. Stay Foolish.\"It was their farewell message as they signed off. Stay Hungry. Stay Foolish. And I have always wished that for myself. And now, as you graduate to begin anew, I wish that for you.Stewart and his team put out several issues of The Whole Earth Catalog, and then when it had run its course, they put out a final issue. It was the mid-1970s, and I was your age. On the back cover of their final issue was a photograph of an early morning country road, the kind you might find yourself hitchhiking on if you were so adventurous. Beneath it were the words: \"Stay Hungry. Stay Foolish.\"It was their farewell message as they signed off. Stay Hungry. Stay Foolish. And I have always wished that for myself. And now, as you graduate to begin anew, I wish that for you.";
	}
</script>
</BODY>
</HTML>