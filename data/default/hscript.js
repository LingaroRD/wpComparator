<script>
//------------------------------------------------------------------
var nodeArray = ["DIV","TABLE","P","H4","H1","H2","H3","H6","H5","A","CENTER","STRONG"];
var counter= {CounterInitial};
var numel = {LayerNumber};
var timeWait = {LayerChangeDelay};
var currentElement=0;
//------------------------------------------------------------------
function onElements(parentNode,elID)
{
var subNodes = parentNode.childNodes;
for (var i=0;i<subNodes.length;++i)
{
	if (nodeArray.indexOf(subNodes[i].nodeName.toUpperCase())>=0)
	{
		if (subNodes[i].attributes.getNamedItem("LinID").value=="L"+elID)
		{
		subNodes[i].style.backgroundColor='green';
		return;
		}
	onElements(subNodes[i],elID);	
	}
}
}
//------------------------------------------------------------------
function offElements(parentNode,elID)
{
var subNodes = parentNode.childNodes;
for (var i=0;i<subNodes.length;++i)
{
	if (nodeArray.indexOf(subNodes[i].nodeName.toUpperCase())>=0)
	{
		if (subNodes[i].attributes.getNamedItem("LinID").value=="L"+elID)
		{
		subNodes[i].style.backgroundColor='';
		return;
		}
	offElements(subNodes[i],elID);	
	}
}
}
//------------------------------------------------------------------
function markElements(parentNode,subLevel)
{
var subNodes = parentNode.childNodes;
for (var i=0;i<subNodes.length;++i)
	{
	if (nodeArray.indexOf(subNodes[i].nodeName.toUpperCase())>=0)
		{
		markElements(subNodes[i],subLevel+1);
		subNodes[i].setAttribute("LinID", "L"+counter);
		counter++;
		}
	}
}
//------------------------------------------------------------------
function timeTick()
{
if (currentElement>0)
	offElements(document.body,currentElement-1);
onElements(document.body,currentElement);
document.title="WebPageComparator-Layer:"+currentElement;
currentElement++;
setTimeout(function(){timeTick();},timeWait);
}
//------------------------------------------------------------------
function start()
{
document.title="WebPageComparator-Layer:0";
markElements(document.body,0);
currentElement = 1;
setTimeout(function(){timeTick();},timeWait*3);
}

</script>