
function seleccionarMascotaJugador() {

    let inputHipodoge = document.getElementById('hipodoge');
    let inputCapipepo = document.getElementById('capipepo');
    let inputRatigueya = document.getElementById('ratigueya');
    let inputLangostelvis = document.getElementById('langostelvis');
    let inputPydos = document.getElementById('pydos');

    if (inputHipodoge.checked) {
        alert('Seleccionaste a Hipodoge');
    } else if (inputCapipepo.checked) {
        alert('Seleccionaste a Capipepo');
    } else if (inputRatigueya.checked) {
        alert('Seleccionaste a Ratigueya');
    } else if (inputLangostelvis.checked) {
        alert('Seleccionaste a Langostelvis');
    } else if (inputPydos.checked) {
        alert('Seleccionaste a Pydos');
    } else {
        alert('Selecciona una mascota');
    }
}

function iniciarJuego() {

    let botonMascotaJugador = document.getElementById('boton-mascotas');
    botonMascotaJugador.addEventListener('click', seleccionarMascotaJugador);

}


window.addEventListener('load', iniciarJuego);


