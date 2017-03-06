/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.

* File Name : Utilities.h

* Purpose :

* Creation Date : 02-23-2017

* Last Modified : Fri 24 Feb 2017 03:20:01 PM EST

* Author : Logan Blevins  

_._._._._._._._._._._._._._._._._._._._._.*/



struct
{
    char *extension;
    char *filetype;
}

extensions [] = 
{
    { "html", "text/html" },
    { "txt", "text/plain" },
    { "jpg", "image/jpeg" },
    { "gif", "image/gif" },
    { 0, 0}
};

