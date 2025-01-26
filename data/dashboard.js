// let temperatureLevel = document.getElementById("temperatureLevel");
let temperatureSensor = document.getElementById("temperatureSensor");
// let humidityLevel = document.getElementById("humidityLevel");
let humiditySensor = document.getElementById("humiditySensor");
// let soilLevel = document.getElementById('soilLevel');
// let soilSensor = document.getElementById('soilSensor');
// let flameLevel = document.getElementById('flameLevel');
// let flameSensor = document.getElementById('flameSensor');
// let smokeLevel = document.getElementById('smokeLevel');
// let smokeSensor = document.getElementById('smokeSensor');
let btnMode = document.getElementById("btnMode");
let btnLights = document.getElementById("btnLights");

let btnLedTapeState = document.getElementById("btnLedTapeState");
let btnLedTapeAnimation = document.getElementById("btnLedTapeAnimation");
let btnLedTapeChange = document.getElementById("btnLedTapeChange");

window.addEventListener("load", () => {
  window.localStorage.clear();
  setInterval(receiveData, 1000);
});

btnLights.addEventListener("click", () => {
  sendReq("/lights");
});

btnMode.addEventListener("click", () => {
  sendReq("/mode");
});

btnLedTapeState.addEventListener("click", () => {
  sendReq("/ledTapeState");
});

btnLedTapeAnimation.addEventListener("click", () => {
  sendReq("/ledTapeAnimation");
});

btnLedTapeChange.addEventListener("click", () => {
  sendReq("/ledTapeChange");
});

function sendReq(url) {
  fetch(url)
    .then((resp) => resp.json())
    .then((resp) => {
      console.log(resp);
      if (resp["status"] == "success") console.log("Pedido Realizado!");
      else alert(resp["message"]);
    })
    .catch((error) => {
      console.error("## ERRO PEGANDO OS DADOS:" + error);
    });
}

function receiveData() {
  fetch("/dados")
    .then((resp) => resp.json())
    .then((resp) => {
      console.log(resp);
      let dados = resp["data"].split("*");
      if (dados[0] == "D") {
        temperatureSensor.innerHTML = dados[1] + "*C";
        humiditySensor.innerHTML = dados[2] + "";

        btnMode.innerHTML = dados[3] == "A" ? "AUTOMÁTICO" : "MANUAL";
        btnLights.innerHTML = dados[4] == "1" ? "APAGAR" : "ACENDER";
        btnLedTapeState.innerHTML = dados[5] == "1" ? "APAGAR LEDs" : "ACENDER LEDs";

        btnLedTapeAnimation.innerHTML = dados[6] == "1" ? "PARAR ANIMAÇÃO" : "LIGAR ANIMAÇÃO";
        btnLedTapeChange.innerHTML = "TROCAR COR";

        document.getElementById("imgLights").src = dados[4] == "1" ? "lampOn.png" : "lampOff.png";
        document.getElementById("imgLedTapeState").src = dados[5] == "1" ? "lampOn.png" : "lampOff.png";
        // document.getElementById("imgLedTapeAnimation").src = dados[6] == "1" ? "lampOn.png" : "lampOff.png";
      }
    })
    .catch((error) => {
      console.error("## ERRO PEGANDO OS DADOS:" + error);
    });
}
