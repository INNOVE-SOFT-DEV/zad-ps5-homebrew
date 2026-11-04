/* ZAD pour la télé : interface 1920x1080 pilotée à la manette.
 * La console envoie la manette comme des touches : Croix=Entrée(13), Rond=Échap(27),
 * flèches=37-40, Triangle=F1(112), Carré=F2(113). Sur PC : flèches, Entrée, Échap, Q/E. */
(function () {
  "use strict";

  var TABS = ["Découvrir", "Carte", "Réservations", "Profil"];
  var VERSION = "ZAD · version 1.0.0 (PS5)";
  var state = {tab: 0, detail: null, reservations: [], focusKey: null};
  var screen = document.getElementById("screen");
  var tabsEl = document.getElementById("tabs");
  var toastEl = document.getElementById("toast");
  var toastTimer = 0;

  function esc(s) {
    return String(s).replace(/[&<>"']/g, function (c) {
      return {"&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;"}[c];
    });
  }
  function formatDA(n) {
    return String(n).replace(/\B(?=(\d{3})+(?!\d))/g, " ") + " DA";
  }
  function byId(id) {
    for (var i = 0; i < PANIERS.length; i++) if (PANIERS[i].id === id) return PANIERS[i];
    return null;
  }
  function ticketCode(id) { return "RV · " + (4000 + (id * 937) % 5000); }
  function reserved(id) {
    for (var i = 0; i < state.reservations.length; i++) if (state.reservations[i] === id) return true;
    return false;
  }

  function toast(msg) {
    toastEl.textContent = msg;
    toastEl.hidden = false;
    clearTimeout(toastTimer);
    toastTimer = setTimeout(function () { toastEl.hidden = true; }, 2200);
  }

  /* ---------------------------------------------------------------- vues */
  function viewTabs() {
    return TABS.map(function (t, i) {
      return '<div class="tab focusable' + (i === state.tab && !state.detail ? " active" : "") +
        '" data-key="tab' + i + '" data-action="tab:' + i + '">' + esc(t) + "</div>";
    }).join("");
  }

  function viewDecouvrir() {
    var cards = PANIERS.map(function (p) {
      return '<div class="card focusable" data-key="p' + p.id + '" data-action="open:' + p.id + '">' +
        '<div class="art" style="background:' + p.couleur + '"><span class="mono">' + esc(p.lettre) + "</span></div>" +
        '<div class="info"><div class="nom">' + esc(p.nom) + '</div><div class="com">' + esc(p.commerce) +
        '</div><div class="prix">' + formatDA(p.prix) + "</div></div></div>";
    }).join("");
    return '<div class="titre">Découvrir</div>' +
      '<div class="search focusable" data-key="search" data-action="search">Chercher un commerce ou un panier</div>' +
      '<div class="grid">' + cards + "</div>";
  }

  function viewDetail(p) {
    var ok = reserved(p.id);
    return '<div class="detail"><div class="art" style="background:' + p.couleur + '"><span class="mono">' + esc(p.lettre) + "</span></div>" +
      '<div class="corps"><h1>' + esc(p.nom) + '</h1><div class="com">' + esc(p.commerce) + "</div>" +
      '<div class="prix">' + formatDA(p.prix) + '</div><div class="creneau">' + esc(p.creneau) + "</div>" +
      "<p>" + esc(p.desc) + " Paiement sur place au retrait.</p>" +
      (ok
        ? '<div class="btn sec focusable" data-key="back" data-action="back">Réservé ✓ — retour</div>'
        : '<div class="btn plein focusable" data-key="reserve" data-action="reserve:' + p.id + '">Réserver</div>') +
      "</div></div>";
  }

  function viewCarte() {
    return '<div class="bientot"><div class="titre">Carte</div><div class="vide">Bientôt disponible</div></div>';
  }

  function viewReservations() {
    var h = '<div class="titre">Réservations</div>';
    if (!state.reservations.length) {
      return h + '<div class="vide">Vous n\'avez pas encore réservé de panier. Choisissez-en un dans Découvrir : il apparaîtra ici.</div>';
    }
    return h + '<div class="liste">' + state.reservations.map(function (id) {
      var p = byId(id);
      return '<div class="ligne focusable" data-key="r' + id + '" data-action="open:' + id + '">' +
        '<div class="pastille" style="background:' + p.couleur + '"><span class="mono">' + esc(p.lettre) + "</span></div>" +
        '<div class="txt"><div class="n">' + esc(p.nom) + '</div><div class="c">' + esc(p.commerce) + " · " + esc(p.creneau) + "</div></div>" +
        '<div class="v">' + formatDA(p.prix) + '</div><div class="ticket">' + ticketCode(id) + "</div></div>";
    }).join("") + "</div>";
  }

  function viewProfil() {
    var items = ["Mes informations", "Moyens de paiement", "Langue", "Aide et signalement"];
    return '<div class="titre">Profil</div><div class="liste menu">' + items.map(function (t, i) {
      return '<div class="ligne focusable" data-key="m' + i + '" data-action="menu:' + i + '">' + esc(t) +
        '<span class="chev">›</span></div>';
    }).join("") + '</div><div class="versionline">' + VERSION + "</div>";
  }

  function render(keepKey) {
    tabsEl.innerHTML = viewTabs();
    var html;
    if (state.detail) html = viewDetail(state.detail);
    else html = [viewDecouvrir, viewCarte, viewReservations, viewProfil][state.tab]();
    screen.innerHTML = html;
    screen.scrollTop = 0;
    var target = keepKey ? document.querySelector('[data-key="' + keepKey + '"]') : null;
    if (!target) target = screen.querySelector(".focusable") || tabsEl.querySelector(".focusable");
    focus(target);
  }

  /* --------------------------------------------------------------- focus */
  function all() {
    return Array.prototype.slice.call(document.querySelectorAll(".focusable"));
  }
  function current() { return document.querySelector(".focusable.focused"); }
  function focus(el) {
    var cur = current();
    if (cur) cur.classList.remove("focused");
    if (!el) return;
    el.classList.add("focused");
    state.focusKey = el.getAttribute("data-key");
    if (screen.contains(el)) {
      var er = el.getBoundingClientRect(), sr = screen.getBoundingClientRect(), m = 40;
      if (er.bottom > sr.bottom - m) screen.scrollTop += er.bottom - sr.bottom + m;
      else if (er.top < sr.top + m) screen.scrollTop -= sr.top + m - er.top;
    }
  }
  function centre(el) {
    var r = el.getBoundingClientRect();
    return {x: r.left + r.width / 2, y: r.top + r.height / 2};
  }
  function move(dir) {
    var cur = current();
    if (!cur) { focus(all()[0]); return; }
    var c = centre(cur), best = null, bestScore = Infinity;
    all().forEach(function (el) {
      if (el === cur) return;
      var p = centre(el), dx = p.x - c.x, dy = p.y - c.y, along, across;
      if (dir === "left") { along = -dx; across = Math.abs(dy); }
      else if (dir === "right") { along = dx; across = Math.abs(dy); }
      else if (dir === "up") { along = -dy; across = Math.abs(dx); }
      else { along = dy; across = Math.abs(dx); }
      if (along <= 4) return;
      var score = along + across * 2.2;
      if (score < bestScore) { bestScore = score; best = el; }
    });
    if (best) focus(best);
  }

  /* ------------------------------------------------------------- actions */
  function setTab(i) {
    state.tab = (i + TABS.length) % TABS.length;
    state.detail = null;
    render("tab" + state.tab);
  }
  function activate(el) {
    if (!el) return;
    var a = (el.getAttribute("data-action") || "").split(":"), name = a[0], arg = a[1];
    if (name === "tab") setTab(parseInt(arg, 10));
    else if (name === "open") { state.detail = byId(parseInt(arg, 10)); render(); }
    else if (name === "reserve") {
      var id = parseInt(arg, 10);
      if (!reserved(id)) state.reservations.push(id);
      toast("Panier réservé · " + ticketCode(id));
      render("back");
    }
    else if (name === "back") back();
    else if (name === "search") toast("La recherche arrive bientôt");
    else if (name === "menu") toast("Bientôt disponible");
  }
  function back() {
    if (state.detail) { var k = "p" + state.detail.id; state.detail = null; render(state.tab === 0 ? k : null); }
  }

  /* -------------------------------------------------------------- entrées */
  var CODES = {13: "ok", 27: "back", 37: "left", 38: "up", 39: "right", 40: "down", 112: "next", 113: "prev"};
  var KEYS = {Enter: "ok", Escape: "back", Esc: "back", Backspace: "back", ArrowLeft: "left", ArrowUp: "up",
              ArrowRight: "right", ArrowDown: "down", F1: "next", F2: "prev", e: "next", E: "next", q: "prev", Q: "prev"};
  var lastRepeat = 0;
  document.addEventListener("keydown", function (e) {
    var act = (e.key && KEYS[e.key]) || CODES[e.keyCode || e.which];
    if (!act) return;
    e.preventDefault();
    if (e.repeat) {
      var now = Date.now();
      if (now - lastRepeat < 90) return;
      lastRepeat = now;
    }
    if (act === "ok") activate(current());
    else if (act === "back") back();
    else if (act === "next") setTab(state.tab + 1);
    else if (act === "prev") setTab(state.tab - 1);
    else move(act);
  }, true);

  function horloge() {
    var d = new Date(), h = d.getHours(), m = d.getMinutes();
    document.getElementById("clock").textContent = (h < 10 ? "0" : "") + h + ":" + (m < 10 ? "0" : "") + m;
  }
  horloge();
  setInterval(horloge, 20000);
  render();
})();
