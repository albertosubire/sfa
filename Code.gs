const SHEET_ID = "1_N-EoLChyEwj_NmkwxeaeW8Iw3lyi7BFP4s_bd9gjFM";
const SHEET_NAME = "Mensajes";
const DESTINO_EMAIL = "signforallsubire@gmail.com";

function getHoja() {
  return SpreadsheetApp.openById(SHEET_ID).getSheetByName(SHEET_NAME);
}

function inicializarHoja() {
  const hoja = getHoja();

  if (hoja.getLastRow() === 0) {
    hoja.appendRow(["ID", "Mensaje", "Estado", "Fecha"]);
  }
}

function doGet(e) {
  inicializarHoja();

  // Abrir dashboard
  if (!e || !e.parameter.accion) {
    return HtmlService.createHtmlOutputFromFile("index");
  }

  const accion = e.parameter.accion;
  const hoja = getHoja();

  // Agregar mensaje nuevo desde ESP32
  if (accion === "agregar") {
    const texto = e.parameter.texto || "Sin texto";

    // insertar nueva fila debajo del encabezado
    hoja.insertRowBefore(2);

    hoja.getRange(2, 1, 1, 4).setValues([[
      new Date().getTime(),
      texto,
      "Nuevo",
      new Date()
    ]]);

    MailApp.sendEmail({
      to: DESTINO_EMAIL,
      subject: "Nuevo mensaje recibido",
      body: texto
    });

    return ContentService.createTextOutput("OK");
  }

  // Marcar último como enviado
  if (accion === "enviar") {
    if (hoja.getLastRow() > 1) {
      hoja.getRange(2, 3).setValue("Enviado");
    }

    return ContentService.createTextOutput("ENVIADO");
  }

  return ContentService.createTextOutput("ERROR");
}

function obtenerDashboard() {
  const hoja = getHoja();
  const datos = hoja.getDataRange().getValues();

  if (datos.length <= 1) {
    return {
      actual: "Sin mensajes",
      estado: "-",
      enviados: 0,
      historial: []
    };
  }

  const historial = datos.slice(1);
  const ultimo = historial[0];
  const enviados = historial.filter(fila => fila[2] === "Enviado").length;

  return {
    actual: ultimo[1],
    estado: ultimo[2],
    enviados: enviados,
    historial: historial
  };
}
