#define SCRIPTJS "function setup(event) { \ 
  console.log(\"begin\");  \ 
  var canvas = document.getElementById(\'myCanvas\'); \ 
  var ctx = canvas.getContext(\'2d\'); \ 
  var eSquare = 20; var gridSize = 50; \ 
  ctx.strokeStyle = \'rgb(0, 0, 0)\'; \ 
  for(let x=1; x<=gridSize; x+=1) { \ 
    for (let y=1; y<=gridSize; y+=1) { \ 
      ctx.strokeRect(x*eSquare,y*eSquare,eSquare,eSquare); \       
    } \ 
  } \ 
  setInterval(fetchJSON, 7000);   \ 
}  \ 
function fetchJSON() {  \ 
  fetch(\'/readings\')   \ 
    .then(response => response.json())    \ 
    .then(data => getResponse(data))     \ 
    .catch(error => getResponse(error));  \ 
}  \ 
function getResponse(data) {   \ 
  console.log(data);  \ 
  var gridElement = 0;  \ 
  for (property in data) {  \ 
    let label = property;  \ 
    let value = data[property];  \ 
    console.log(label);  \ 
    console.log(value);  \ 
    if (document.getElementById(label) != null) {  \ 
      document.getElementById(label).innerHTML = value;   \ 
    } else if (label.includes(\"gElem\")) {  \ 
      gridElement = Number(value);  \ 
    } else if (label.includes(\"gType\")) {  \
      drawGridPoint(gridElement, value); \ 
    }  \ 
  }  \ 
}  \ 
function drawGridPoint(elem, type) { \ 
  var canvas = document.getElementById(\'myCanvas\'); \ 
  var ctx = canvas.getContext(\'2d\'); \ 
  var eSquare = 20; var fSquare = 17; var gridSize = 50; \ 
  var x = parseInt(elem / gridSize) + 1; \ 
  var y = elem - ((x - 1) * gridSize); \ 
  if (type == \"1\") { \ 
    ctx.fillStyle = \'rgb(183, 40, 202)\'; \ 
  } else if (type == \"2\") { \ 
    ctx.fillStyle = \'rgb(36, 199, 15)\'; \ 
  } else if (type == \"3\") { \ 
    ctx.fillStyle = \'rgb(255, 0, 0)\'; \ 
  } else { \ 
    ctx.fillStyle = \'rgb(0, 0, 0)\'; \ 
  } \ 
  ctx.fillRect(x*eSquare+1,y*eSquare+1,fSquare,fSquare); \ 
  console.log(type); \ 
} \ 
window.addEventListener(\'DOMContentLoaded\', setup);"