// UI Controller and Event Coordinator for YaarLang Workbench
document.addEventListener('DOMContentLoaded', () => {
    const editor = document.getElementById('code-editor');
    const lineNumbers = document.getElementById('line-numbers');
    const sampleSelect = document.getElementById('sample-select');
    const btnRun = document.getElementById('btn-run');
    const btnReset = document.getElementById('btn-reset');
    const deckContent = document.getElementById('deck-content');
    const tabButtons = document.querySelectorAll('.tab-btn');

    const runtime = new YaarLangRuntime();
    let currentCompilation = null;
    let activeTab = 'terminal';

    // 1. Update Line Numbers
    function updateLineNumbers() {
        const lines = editor.value.split('\n').length;
        lineNumbers.innerHTML = Array.from({ length: lines }, (_, i) => i + 1).join('<br>');
    }

    editor.addEventListener('input', updateLineNumbers);
    editor.addEventListener('scroll', () => {
        lineNumbers.scrollTop = editor.scrollTop;
    });

    // 2. Load Sample
    function loadSample(key) {
        if (typeof SAMPLES !== 'undefined' && SAMPLES[key]) {
            editor.value = SAMPLES[key];
            updateLineNumbers();
            executeCode();
        }
    }

    sampleSelect.addEventListener('change', (e) => {
        loadSample(e.target.value);
    });

    btnReset.addEventListener('click', () => {
        loadSample(sampleSelect.value);
    });

    // 3. Tab Switching
    tabButtons.forEach(btn => {
        btn.addEventListener('click', () => {
            tabButtons.forEach(b => b.classList.remove('active'));
            btn.classList.add('active');
            activeTab = btn.getAttribute('data-tab');
            renderDeckContent();
        });
    });

    // 4. Render Active Tab Content
    function renderDeckContent() {
        if (!currentCompilation) {
            deckContent.textContent = "Click 'Chalao (Run)' to compile and execute.";
            return;
        }

        if (!currentCompilation.success) {
            deckContent.innerHTML = `<span style="color: #ef4444;">${escapeHTML(currentCompilation.error)}</span>`;
            return;
        }

        switch (activeTab) {
            case 'terminal':
                deckContent.textContent = currentCompilation.output || "[Program completed with no output]";
                break;
            case 'ast':
                deckContent.textContent = currentCompilation.ast || "[AST Empty]";
                break;
            case 'tac':
                deckContent.textContent = currentCompilation.tac || "[TAC Empty]";
                break;
            case 'mips':
                deckContent.textContent = currentCompilation.mips || "[MIPS Empty]";
                break;
            case 'opt':
                deckContent.textContent = currentCompilation.opt || "[Optimization Stats Empty]";
                break;
        }
    }

    function escapeHTML(str) {
        return str.replace(/[&<>'"]/g, 
            tag => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', "'": '&#39;', '"': '&quot;' }[tag] || tag));
    }

    // 5. Execute Code
    function executeCode() {
        const code = editor.value;
        currentCompilation = runtime.compileAndRun(code);
        renderDeckContent();
    }

    btnRun.addEventListener('click', executeCode);

    // Initial Load
    loadSample('fibonacci');
});
