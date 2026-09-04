#pragma once

// After edit webpage.html, paste the content to R"rawliteral([content]])".

const char WEBPAGE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <style>
     * { box-sizing: border-box; -webkit-tap-highlight-color: transparent; }

    html, body {
      margin: 0;
      padding: 0;
      width: 100%;
      height: 100%;
      overflow: hidden;
      background: #353131; /* Fundo cinza caso o vídeo falhe */
      display: flex;
      align-items: center;
      justify-content: center;
      touch-action: manipulation;
      font-family: Arial, sans-serif;
    }

    /* ---- Container do Vídeo (Fundo em Tela Cheia) ---- */
    .video-container {
       position: absolute;
	   top: 50%;
	   left: 50%;
	   width: 420px;
	   height: 420px;
	   transform: translate(-50%, -50%);
	   z-index: 1;
    }

    /* O elemento de imagem agora preenche toda a tela sem distorcer */
    .stream {
      width: 100%;
      height: 100%;
      object-fit: cover; 
      border: none !important;
      border-radius: 0 !important;
    }

    /* ---- Wrapper do Controle ---- */
    .controller-wrapper {
      display: flex;
      align-items: flex-end;
      justify-content: center;
      width: 100%;
      height: 100%;
      position: relative;
      z-index: 2; /* Força o controle a ficar na frente do vídeo */
      pointer-events: none; /* Permite toques no fundo se necessário */
    }

    /* ---- O Controle Transparente ---- */
    .controller {
      width: 100%;
      transform-origin: center;
      pointer-events: auto; /* Devolve os cliques apenas para o controle */
      padding: 20px 24px;
      position: relative;
    }

    .controller-inner {
      display: flex;
      align-items: center;
      justify-content: space-between;
      height: 100%;
    }

    /* ---- D-pad (Levemente mais visível no fundo claro/escuro) ---- */
    .dpad {
      position: relative;
      width: 96px;
      height: 96px;
      flex-shrink: 0;
    }

    .dpad-arm {
      position: absolute;
      background: rgba(17, 17, 17, 0.85); /* Preto levemente transparente */
      border-radius: 2px;
      transition: transform 0.05s ease, filter 0.05s ease;
    }

    .dpad-arm-up    { top: 0; left: 32px; width: 32px; height: 32px; }
    .dpad-arm-down  { bottom: 0; left: 32px; width: 32px; height: 32px; }
    .dpad-arm-left  { top: 32px; left: 0; width: 32px; height: 32px; }
    .dpad-arm-right { top: 32px; right: 0; width: 32px; height: 32px; }

	.left-btns {
	  justify-content: flex-start;
      display: flex;
      flex-direction: row;
      align-items: center;
      gap: 30px;
	}
	
    .dpad-center {
      position: absolute;
      top: 50%;
      left: 50%;
      transform: translate(-50%, -50%);
      width: 32px;
      height: 32px;
      background: rgba(17, 17, 17, 0.85);
      z-index: 0;
      pointer-events: none;
    }

    .dpad:has(.dpad-up:active)    .dpad-arm-up    { transform: translateY(2px);  filter: brightness(1.5); }
    .dpad:has(.dpad-down:active)  .dpad-arm-down  { transform: translateY(-2px); filter: brightness(1.5); }
    .dpad:has(.dpad-left:active)  .dpad-arm-left  { transform: translateX(2px);  filter: brightness(1.5); }
    .dpad:has(.dpad-right:active) .dpad-arm-right { transform: translateX(-2px); filter: brightness(1.5); }

    .dpad-btn {
      position: absolute;
      background: transparent;
      border: none;
      cursor: pointer;
      touch-action: none;
      z-index: 1;
    }

    .dpad-up    { top: 0;    left: 32px; width: 32px; height: 32px; }
    .dpad-down  { bottom: 0; left: 32px; width: 32px; height: 32px; }
    .dpad-left  { top: 32px; left: 0;    width: 32px; height: 32px; }
    .dpad-right { top: 32px; right: 0; width: 32px; height: 32px; }

    /* ---- Start / Select ---- */
    .center-buttons {
      display: flex;
      flex-direction: column;
      align-items: center;
	  text-align: center;
      gap: 10px;
    }

    .pill-btn {
      width: 48px;
      height: 14px;
      background: rgba(50, 50, 50, 0.8);
      border: none;
      border-radius: 7px;
      cursor: pointer;
      touch-action: none;
      box-shadow: inset 0 2px 4px rgba(0, 0, 0, 0.4);
    }

    .pill-btn:active { background: #777; }

    .pill-label {
      font-size: 9px;
      color: #fff; /* Mudado para branco para destacar melhor sobre o vídeo */
      text-shadow: 1px 1px 2px rgba(0,0,0,0.8);
      letter-spacing: 0.05em;
      margin-top: 2px;
    }

    /* ---- Face buttons (Opacidade nos botões coloridos) ---- */
    .face-buttons {
      position: relative;
      width: 108px;
      height: 124px;
      flex-shrink: 0;
    }

    .face-group {
      position: absolute;
    }

    .face-btn {
      width: 40px;
      height: 40px;
      border-radius: 50%;
      border: none;
      cursor: pointer;
      touch-action: none;
      font-size: 14px;
      font-weight: bold;
      color: #fff;
      transition: transform 0.05s ease, box-shadow 0.05s ease, filter 0.05s ease;
    }

    .face-btn:active {
      transform: translateY(2px);
      filter: brightness(1.15);
    }

    .face-y { top: 0;  left: 50px; }
    .face-x { top: 34px; left: 0; }
    .face-a { top: 50px; right: 0; }
    .face-b { bottom: 0; left: 15px; }

    /* Cores originais preservadas com 65% de opacidade */
    .btn-y {
      background: rgba(125, 60, 152, 0.65);
      box-shadow: 0 3px 0 #4a235a, 0 4px 8px rgba(0, 0, 0, 0.3);
    }
    .btn-x {
      background: rgba(36, 113, 163, 0.65);
      box-shadow: 0 3px 0 #1a5276, 0 4px 8px rgba(0, 0, 0, 0.3);
    }
    .btn-a {
      background: rgba(231, 76, 60, 0.65);
      box-shadow: 0 3px 0 #8b00007d, 0 4px 8px rgba(0, 0, 0, 0.3);
    }
    .btn-b {
      background: rgba(241, 196, 15, 0.65);
      color: #333;
      box-shadow: 0 3px 0 #b7950b, 0 4px 8px rgba(0, 0, 0, 0.3);
    }

    .btn-y:active { box-shadow: 0 1px 0 #4a235a, 0 2px 4px rgba(0, 0, 0, 0.3); }
    .btn-x:active { box-shadow: 0 1px 0 #1a5276, 0 2px 4px rgba(0, 0, 0, 0.3); }
    .btn-a:active { box-shadow: 0 1px 0 #8b0000, 0 2px 4px rgba(0, 0, 0, 0.3); }
    .btn-b:active { box-shadow: 0 1px 0 #b7950b, 0 2px 4px rgba(0, 0, 0, 0.3); }
	
    /* ---- Texto de Mensagem Flutuante ---- */
    .mensagem-text {
      position: absolute;
      font-size: 11px;
      color: #fff;
      text-shadow: 1px 1px 2px rgba(0,0,0,0.8);
	  width: 100%;
	  padding: 2px;
	  background: rgba(220, 220, 220, 0.4); 
      backdrop-filter: blur(10px); 
      -webkit-backdrop-filter: blur(10px);
    }
  </style>
  <script>
	const host = location.hostname || "localhost";
	
    const socket = new WebSocket("ws://" + host + ":81/");
	
    socket.onmessage = function(event) {
      document.getElementById("mensagem").textContent = event.data;
    };
	
    function sendCommand(command) {
      if (socket.readyState !== WebSocket.OPEN) {
        console.error("WebSocket não conectado");
        return;
      }
      socket.send(JSON.stringify(command));
    }

    function bindButton(el, name) {
      const press   = () => sendCommand({ name, action: "press" });
      const release = () => sendCommand({ name, action: "release" });

      el.addEventListener("mousedown", press);
      el.addEventListener("mouseup", release);
      el.addEventListener("mouseleave", release);

      el.addEventListener("touchstart", e => { e.preventDefault(); press(); });
      el.addEventListener("touchend",   e => { e.preventDefault(); release(); });
      el.addEventListener("touchcancel", e => { e.preventDefault(); release(); });
    }

    window.onload = function() {
      // Bind dos botões
      bindButton(document.getElementById("upBtn"),     "up");
      bindButton(document.getElementById("downBtn"),   "down");
      bindButton(document.getElementById("leftBtn"),   "left");
      bindButton(document.getElementById("rightBtn"),  "right");
      bindButton(document.getElementById("aBtn"),      "a");
      bindButton(document.getElementById("bBtn"),      "b");
      bindButton(document.getElementById("xBtn"),      "x");
      bindButton(document.getElementById("yBtn"),      "y");
      bindButton(document.getElementById("startBtn"),  "start");
      bindButton(document.getElementById("selectBtn"), "select");

    };
	
    // ---- Video WebSocket (Port 82) ----
    const streamSocket = new WebSocket("ws://" + host + ":82/");
    streamSocket.binaryType = "blob";

    let displaying = false;
	
    streamSocket.onmessage = async (event) => {
      if (displaying) {
        return;
      }

      const stream = document.getElementById("stream");
      if (!stream) return; 

      displaying = true;

      const url = URL.createObjectURL(event.data);

      stream.onload = () => {
        URL.revokeObjectURL(url);
        displaying = false;
      };

      stream.src = url;
    };
	
  </script>
</head>
<body>

  <div class="video-container">
	<img id="stream" class="stream" src="https://c4.wallpaperflare.com/wallpaper/605/299/689/cat-fish-mermaid-aquarium-wallpaper-preview.jpg" alt="Aguardando stream...">
  </div>
	
  <div class="controller-wrapper">
  
	<div id="mensagem" class="mensagem-text">Aguardando mensagem...</div>
	
    <div class="controller">
      <div class="controller-inner">

		<div class="left-btns">

			<!-- D-pad -->
			<div class="dpad">
			  <div class="dpad-arm dpad-arm-up"></div>
			  <div class="dpad-arm dpad-arm-down"></div>
			  <div class="dpad-arm dpad-arm-left"></div>
			  <div class="dpad-arm dpad-arm-right"></div>
			  <div class="dpad-center"></div>
			  <button id="upBtn"    class="dpad-btn dpad-up"    aria-label="Up"></button>
			  <button id="downBtn"  class="dpad-btn dpad-down"  aria-label="Down"></button>
			  <button id="leftBtn"  class="dpad-btn dpad-left"  aria-label="Left"></button>
			  <button id="rightBtn" class="dpad-btn dpad-right" aria-label="Right"></button>
			</div>

			<!-- Start / Select -->
			<div class="center-buttons">
			  <div>
				<button id="selectBtn" class="pill-btn" aria-label="Select"></button>
				<div class="pill-label">SELECT</div>
			  </div>
			  <div>
				<button id="startBtn" class="pill-btn" aria-label="Start"></button>
				<div class="pill-label">START</div>
			  </div>
			</div>
		</div>
        <!-- X / Y / A / B -->
        <div class="face-buttons">
          <div class="face-group face-y">
            <button id="yBtn" class="face-btn btn-y" aria-label="Y">Y</button>
          </div>
          <div class="face-group face-x">
            <button id="xBtn" class="face-btn btn-x" aria-label="X">X</button>
          </div>
          <div class="face-group face-a">
            <button id="aBtn" class="face-btn btn-a" aria-label="A">A</button>
          </div>
          <div class="face-group face-b">
            <button id="bBtn" class="face-btn btn-b" aria-label="B">B</button>
          </div>
        </div>

      </div>
	  
	</div>
  </div>
</body>
</html>
)rawliteral";
