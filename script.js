var commentsHidden = false;
var classesHidden = false;
var functionsHidden = false;
function toggleVisibility(classname)
{
	var elements = document.getElementsByClassName(classname);
	if (classname == "comments")
	{
		if(commentsHidden) commentsHidden = false;
		else commentsHidden = true;
		var i;
		for (i = 0; i < elements.length; i++)
		{
			elements[i].hidden = commentsHidden;
		}
	}

	else if (classname == "class")
	{
		if(classesHidden) classesHidden = false;
		else classesHidden = true;
		var i;
		for (i = 0; i < elements.length; i++)
		{
			elements[i].hidden = classesHidden;
		}
	}

	else if (classname == "function")
	{
		if(functionsHidden) functionsHidden = false;
		else functionsHidden = true;
		var i;
		for (i = 0; i < elements.length; i++)
		{
			elements[i].hidden = functionsHidden;
		}
	}
}