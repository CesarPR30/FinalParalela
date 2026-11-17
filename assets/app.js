(function () {
  var params = new URLSearchParams(location.search);
  var printBlock = params.get('print'); // ?print=b1 -> modo PDF

  // KaTeX: renderiza \( \) y \[ \]
  function renderMath() {
    if (window.renderMathInElement) {
      renderMathInElement(document.body, {
        delimiters: [
          { left: '\\[', right: '\\]', display: true },
          { left: '\\(', right: '\\)', display: false }
        ],
        throwOnError: false
      });
      document.documentElement.setAttribute('data-math', 'ok');
    }
  }
  if (document.readyState === 'loading') document.addEventListener('DOMContentLoaded', renderMath); else renderMath();

  // Tarjetas plegables
  document.querySelectorAll('.card > button').forEach(function (b) {
    b.setAttribute('aria-expanded', 'false');
    b.addEventListener('click', function () {
      var o = b.parentElement.classList.toggle('open');
      b.setAttribute('aria-expanded', o ? 'true' : 'false');
    });
  });
  document.querySelectorAll('[data-open-all]').forEach(function (btn) {
    btn.addEventListener('click', function () {
      var scope = document.getElementById(btn.getAttribute('data-open-all'));
      var open = btn.getAttribute('data-mode') !== 'close';
      scope.querySelectorAll('.card').forEach(function (c) { c.classList.toggle('open', open); });
    });
  });

  // Pestañas de bloque (index)
  var tabs = document.querySelectorAll('.tab');
  function showBlock(id) {
    if (!document.getElementById(id)) return;
    document.querySelectorAll('.block').forEach(function (b) { b.classList.toggle('on', b.id === id); });
    tabs.forEach(function (t) { t.classList.toggle('on', t.getAttribute('data-b') === id); t.setAttribute('aria-selected', t.getAttribute('data-b') === id ? 'true' : 'false'); });
  }
  if (tabs.length) {
    tabs.forEach(function (t) {
      t.addEventListener('click', function () {
        var id = t.getAttribute('data-b');
        showBlock(id);
        try { history.replaceState(null, '', '#' + id); } catch (e) { location.hash = id; }
        window.scrollTo({ top: document.querySelector('.tabs').offsetTop - 70 });
      });
    });
    var h = (location.hash || '').replace('#', '');
    var start = printBlock || (/^b[123]/.test(h) ? h.slice(0, 2) : 'b1');
    showBlock(start);
    if (/^b[123]-/.test(h)) { var el = document.getElementById(h); if (el) setTimeout(function () { el.scrollIntoView(); }, 50); }
    window.addEventListener('hashchange', function () {
      var hh = location.hash.replace('#', '');
      if (/^b[123]/.test(hh)) showBlock(hh.slice(0, 2));
    });
  }

  // Exámenes: mostrar resolución / cómo hacerlo
  document.querySelectorAll('[data-toggle]').forEach(function (btn) {
    btn.addEventListener('click', function () {
      var p = document.getElementById(btn.getAttribute('data-toggle'));
      var on = p.classList.toggle('on');
      btn.classList.toggle('on', on);
      btn.setAttribute('aria-expanded', on ? 'true' : 'false');
    });
  });
  var allSol = document.getElementById('allSol'), allHow = document.getElementById('allHow'), hideAll = document.getElementById('hideAll');
  function setPanels(sel, on) {
    document.querySelectorAll(sel).forEach(function (p) {
      p.classList.toggle('on', on);
      var b = document.querySelector('[data-toggle="' + p.id + '"]');
      if (b) b.classList.toggle('on', on);
    });
  }
  if (allSol) allSol.addEventListener('click', function () { setPanels('.panel.sol', true); });
  if (allHow) allHow.addEventListener('click', function () { setPanels('.panel.how', true); });
  if (hideAll) hideAll.addEventListener('click', function () { setPanels('.panel', false); });
  // filtro por bloque en exámenes
  document.querySelectorAll('[data-filter]').forEach(function (btn) {
    btn.addEventListener('click', function () {
      var f = btn.getAttribute('data-filter');
      document.querySelectorAll('[data-filter]').forEach(function (b) { b.classList.toggle('on', b === btn); });
      document.querySelectorAll('.q').forEach(function (q) {
        q.hidden = !(f === 'all' || q.getAttribute('data-b') === f);
      });
    });
  });

  // Checklist con localStorage
  var boxes = document.querySelectorAll('.check input');
  if (boxes.length) {
    var KEY = 'final-paralela-check-v1', state = {};
    try { state = JSON.parse(localStorage.getItem(KEY) || '{}') || {}; } catch (e) { state = {}; }
    function upd() {
      document.querySelectorAll('.check').forEach(function (list) {
        var bs = list.querySelectorAll('input'), n = 0;
        bs.forEach(function (x) { if (x.checked) n++; });
        var bar = document.getElementById(list.getAttribute('data-bar'));
        if (bar) bar.style.width = (100 * n / bs.length) + '%';
      });
    }
    boxes.forEach(function (x) {
      if (state[x.id]) x.checked = true;
      x.addEventListener('change', function () {
        state[x.id] = x.checked;
        try { localStorage.setItem(KEY, JSON.stringify(state)); } catch (e) {}
        upd();
      });
    });
    upd();
  }

  // Modo PDF: abre todo
  if (printBlock) {
    document.documentElement.classList.add('printing');
    var tb = document.querySelector('.tab[data-b="' + printBlock + '"]');
    if (tb) {
      document.querySelector('h1').textContent = 'Repaso final · ' + tb.querySelector('.t').textContent;
      document.querySelector('.hero .eyebrow').textContent = tb.querySelector('.k').textContent + ' · CS4052 Computación Paralela y Distribuida';
    }
    document.querySelectorAll('.card').forEach(function (c) { c.classList.add('open'); });
    document.querySelectorAll('.panel').forEach(function (p) { p.classList.add('on'); });
  }
})();
