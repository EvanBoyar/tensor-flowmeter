// TEST VERSION - Uses localStorage instead of ESP32 backend
// This allows testing the UI/UX without hardware
// Uses real API pricing to accurately simulate water consumption

// DOM Elements
const chatMessages = document.getElementById('chatMessages');
const messageInput = document.getElementById('messageInput');
const sendBtn = document.getElementById('sendBtn');
const newSessionBtn = document.getElementById('newSessionBtn');
const settingsBtn = document.getElementById('settingsBtn');
const settingsPanel = document.getElementById('settingsPanel');
const closeSettingsBtn = document.getElementById('closeSettingsBtn');
const saveSettingsBtn = document.getElementById('saveSettingsBtn');
const waterValue = document.getElementById('waterValue');
const statusText = document.getElementById('statusText');

// Settings inputs
const apiKeyInput = document.getElementById('apiKey');
const voltageInput = document.getElementById('voltage');
const vThresholdInput = document.getElementById('vThreshold');
const conductivityInput = document.getElementById('conductivity');
const areaInput = document.getElementById('area');
const gapInput = document.getElementById('gap');
const safetyResistorInput = document.getElementById('safetyResistor');
const costPerSecondInput = document.getElementById('costPerSecond');
const maxDurationInput = document.getElementById('maxDuration');
const maxWaterInput = document.getElementById('maxWater');

// State
let isSending = false;
let totalWaterUsed = 0;
let waterBaseline = 0; // Water at start of current electrolysis
let isSimulatingElectrolysis = false;
let electrolysisStartTime = 0;
let electrolysisDuration = 0;

// Initialize
document.addEventListener('DOMContentLoaded', () => {
    loadSettings();
    setupEventListeners();
    autoResizeTextarea();
    startWaterSimulation();

    // Show helpful message in console
    console.log('%c🧪 TEST MODE', 'color: #d97757; font-size: 20px; font-weight: bold');
    console.log('This is a test version using localStorage.');
    console.log('Settings will persist in your browser.');
    console.log('No actual API calls will be made.');
    console.log('Watch the Tensor Flow Meter update in real-time!');
});

// Event Listeners
function setupEventListeners() {
    sendBtn.addEventListener('click', sendMessage);
    messageInput.addEventListener('keydown', (e) => {
        if (e.key === 'Enter' && !e.shiftKey) {
            e.preventDefault();
            sendMessage();
        }
    });
    messageInput.addEventListener('input', autoResizeTextarea);

    newSessionBtn.addEventListener('click', startNewSession);
    settingsBtn.addEventListener('click', openSettings);
    closeSettingsBtn.addEventListener('click', closeSettings);
    saveSettingsBtn.addEventListener('click', saveSettings);
}

// Auto-resize textarea
function autoResizeTextarea() {
    messageInput.style.height = 'auto';
    messageInput.style.height = Math.min(messageInput.scrollHeight, 200) + 'px';
}

// Send message (MOCK for testing)
async function sendMessage() {
    if (isSending) return;

    const message = messageInput.value.trim();
    if (!message) return;

    // Check if API key is set
    const settings = JSON.parse(localStorage.getItem('waterMeterSettings') || '{}');
    if (!settings.apiKey || settings.apiKey.length < 10) {
        statusText.textContent = 'Error: API key not configured';
        statusText.style.color = '#ef4444';
        setTimeout(() => {
            statusText.textContent = 'Ready';
            statusText.style.color = '';
        }, 3000);
        return;
    }

    isSending = true;
    sendBtn.disabled = true;
    statusText.textContent = 'Sending...';

    // Add user message to chat
    addMessage('user', message);
    messageInput.value = '';
    autoResizeTextarea();

    // Simulate API delay
    await new Promise(resolve => setTimeout(resolve, 1000 + Math.random() * 1000));

    // Mock response
    const mockResponses = [
        "This is a test response. In the real system, this would be Claude's actual reply from the Anthropic API.",
        "I understand you're testing the interface! This mock response simulates what you'd see from Claude.",
        "The interface looks great! Remember, this is just a preview - the real system will connect to Claude's API.",
        "Testing, testing! The Tensor Flow Meter would be tracking water consumption if this were connected to actual hardware.",
        "In production, I would generate contextual responses based on your message. For now, enjoy exploring the UI!"
    ];

    const response = mockResponses[Math.floor(Math.random() * mockResponses.length)];

    // Mock token counts and cost
    // Using real API pricing: $3/1M input tokens, $15/1M output tokens
    const inputTokens = Math.floor(message.length / 4) + Math.floor(Math.random() * 10);
    const outputTokens = Math.floor(response.length / 4) + Math.floor(Math.random() * 20);
    const cost = (inputTokens * 0.000003) + (outputTokens * 0.000015);

    // Add assistant message
    addMessage('assistant', response, {
        cost: cost,
        inputTokens: inputTokens,
        outputTokens: outputTokens
    });

    // Simulate electrolysis based on cost
    simulateElectrolysis(cost);

    statusText.textContent = 'Ready';
    statusText.style.color = '';

    isSending = false;
    sendBtn.disabled = false;
    messageInput.focus();
}

// Simulate electrolysis (MOCK for testing)
function simulateElectrolysis(cost) {
    const settingsJSON = localStorage.getItem('waterMeterSettings') || '{}';
    const settings = JSON.parse(settingsJSON);

    // Use defaults if settings not saved yet
    const costPerSecond = settings.costPerSecond || 0.00005;
    const voltage = settings.voltage || 5.0;
    const safetyResistor = settings.safetyResistor || 10.0;
    const conductivity = settings.conductivity || 1.0;

    console.log('🔍 Debug - simulateElectrolysis called:');
    console.log('   Cost: $' + cost.toFixed(6));
    console.log('   Settings:', settings);
    console.log('   Cost per second: $' + costPerSecond + '/s');

    // Calculate duration based on cost
    let duration = cost / costPerSecond;

    console.log('   Calculated duration:', duration.toFixed(2), 'seconds');
    console.log('   Settings: V=' + voltage + 'V, R=' + safetyResistor + 'Ω, σ=' + conductivity + 'S/m');

    // Apply max duration limit
    const maxDuration = settings.maxDuration || 300;
    if (duration > maxDuration) {
        duration = maxDuration;
    }

    if (duration > 0.1) {
        // Save current water as baseline for this event
        waterBaseline = totalWaterUsed;

        isSimulatingElectrolysis = true;
        electrolysisStartTime = Date.now();
        electrolysisDuration = duration * 1000; // Convert to milliseconds

        console.log(`💧 Starting electrolysis for ${duration.toFixed(2)} seconds`);
        console.log(`   Cost: $${cost.toFixed(6)} | Cost/sec: $${costPerSecond}/s`);
        console.log(`   Water baseline: ${(waterBaseline * 1000000).toFixed(3)}µg`);

        // Calculate expected water for this event
        const expectedWater = (9.328e-5 * (voltage - (settings.vThreshold || 2.0)) * conductivity * (settings.area || 10.0) * duration) /
                              (safetyResistor * conductivity * (settings.area || 10.0) + 100.0 * (settings.gap || 2.0));
        console.log(`   Expected water for this event: ${(expectedWater * 1000000).toFixed(3)}µg`);

        // Visual feedback
        waterValue.style.animation = 'pulse 1s ease-in-out infinite';
    } else {
        console.log('❌ Duration too short (< 0.1s), skipping electrolysis');
    }
}

// Water simulation update loop
function startWaterSimulation() {
    setInterval(() => {
        if (isSimulatingElectrolysis) {
            const elapsed = Date.now() - electrolysisStartTime;

            if (elapsed >= electrolysisDuration) {
                // Stop simulation
                isSimulatingElectrolysis = false;
                waterValue.style.animation = '';
                const micrograms = totalWaterUsed * 1000000;
                console.log(`✓ Electrolysis complete. Total water: ${totalWaterUsed.toFixed(9)}g (${micrograms.toFixed(3)}µg)`);
            } else {
                // Update water based on elapsed time
                const settings = JSON.parse(localStorage.getItem('waterMeterSettings') || '{}');
                const elapsedSeconds = elapsed / 1000;

                // Calculate water using the formula for THIS event
                const V = settings.voltage || 5.0;
                const V_th = settings.vThreshold || 2.0;
                const sigma = settings.conductivity || 1.0;
                const A = settings.area || 10.0;
                const d = settings.gap || 2.0;
                const R_safety = settings.safetyResistor || 10.0;

                const numerator = 9.328e-5 * (V - V_th) * sigma * A * elapsedSeconds;
                const denominator = R_safety * sigma * A + 100.0 * d;

                const currentEventWater = numerator / denominator;

                // Add to baseline to get cumulative total
                totalWaterUsed = waterBaseline + currentEventWater;

                // Update display (convert to micrograms for visibility)
                const micrograms = totalWaterUsed * 1000000;
                waterValue.textContent = micrograms.toFixed(6) + ' µg';

                // Debug logging every 5 seconds
                if (Math.floor(elapsedSeconds) % 5 === 0 && Math.floor(elapsedSeconds) !== 0) {
                    console.log(`⏱️  Electrolysis progress: ${elapsedSeconds.toFixed(1)}s / ${(electrolysisDuration/1000).toFixed(1)}s | Water: ${micrograms.toFixed(3)}µg`);
                }
            }
        }
    }, 100); // Update every 100ms for smooth animation
}

// Add message to chat
function addMessage(role, content, meta = null) {
    // Remove welcome message if it exists
    const welcomeMessage = chatMessages.querySelector('.welcome-message');
    if (welcomeMessage) {
        welcomeMessage.remove();
    }

    const messageDiv = document.createElement('div');
    messageDiv.className = `message ${role}`;

    const roleDiv = document.createElement('div');
    roleDiv.className = 'message-role';
    roleDiv.textContent = role.charAt(0).toUpperCase() + role.slice(1);

    const contentDiv = document.createElement('div');
    contentDiv.className = 'message-content';
    contentDiv.textContent = content;

    messageDiv.appendChild(roleDiv);
    messageDiv.appendChild(contentDiv);

    // Add metadata for assistant messages
    if (meta && role === 'assistant') {
        const metaDiv = document.createElement('div');
        metaDiv.className = 'message-meta';
        metaDiv.innerHTML = `
            Cost: $${meta.cost.toFixed(6)} |
            Tokens: ${meta.inputTokens} in, ${meta.outputTokens} out
        `;
        messageDiv.appendChild(metaDiv);
    }

    chatMessages.appendChild(messageDiv);

    // Scroll to bottom - need to scroll the parent container, not chatMessages itself
    // The .chat-container parent has overflow-y: auto
    requestAnimationFrame(() => {
        const container = chatMessages.parentElement;
        container.scrollTo({
            top: container.scrollHeight,
            behavior: 'smooth'
        });
    });
}

// Start new session
function startNewSession() {
    if (!confirm('Start a new session? This will clear the current conversation.')) {
        return;
    }

    // Stop any active electrolysis
    isSimulatingElectrolysis = false;
    waterValue.style.animation = '';

    // Clear chat
    chatMessages.innerHTML = `
        <div class="welcome-message">
            <h2>Welcome to AI Water User - Test Version</h2>
            <p>This project makes AI resource consumption tangible by destroying water through electrolysis. Each AI response triggers a reaction proportional to the computational cost, literally turning invisible electricity usage into visible water loss.</p>
            <p><strong>Note:</strong> This is a test version using mock API responses and localStorage. No actual API calls will be made.</p>
            <p><strong>To explore the interface:</strong> Start typing a message below and watch the simulated water consumption in real-time.</p>
        </div>
    `;

    // Reset water display
    totalWaterUsed = 0;
    waterBaseline = 0;
    waterValue.textContent = '0.000000 µg';
    statusText.textContent = 'New session started';
    console.log('🔄 Session reset - water counter cleared');
    setTimeout(() => {
        statusText.textContent = 'Ready';
    }, 2000);
}

// Settings management
function openSettings() {
    settingsPanel.classList.remove('hidden');
}

function closeSettings() {
    settingsPanel.classList.add('hidden');
}

function loadSettings() {
    // Load from localStorage or use defaults
    const savedSettings = localStorage.getItem('waterMeterSettings');

    if (savedSettings) {
        try {
            const settings = JSON.parse(savedSettings);

            apiKeyInput.value = settings.apiKey || '';
            voltageInput.value = settings.voltage || 5.0;
            vThresholdInput.value = settings.vThreshold || 2.0;
            conductivityInput.value = settings.conductivity || 1.0;
            areaInput.value = settings.area || 10.0;
            gapInput.value = settings.gap || 2.0;
            safetyResistorInput.value = settings.safetyResistor || 10.0;
            costPerSecondInput.value = settings.costPerSecond || 0.00005;
            maxDurationInput.value = settings.maxDuration || 300;
            maxWaterInput.value = settings.maxWaterPerSession || 100;

            console.log('✓ Settings loaded from localStorage');
        } catch (error) {
            console.error('Error loading settings:', error);
            setDefaultValues();
        }
    } else {
        // First time - values already set in HTML defaults
        console.log('ℹ No saved settings found, using defaults');
    }
}

function setDefaultValues() {
    // These match the HTML defaults
    voltageInput.value = 5.0;
    vThresholdInput.value = 2.0;
    conductivityInput.value = 1.0;
    areaInput.value = 10.0;
    gapInput.value = 2.0;
    safetyResistorInput.value = 10.0;
    costPerSecondInput.value = 0.00005;
    maxDurationInput.value = 300;
    maxWaterInput.value = 100;
}

function saveSettings() {
    const settings = {
        apiKey: apiKeyInput.value,
        voltage: parseFloat(voltageInput.value),
        vThreshold: parseFloat(vThresholdInput.value),
        conductivity: parseFloat(conductivityInput.value),
        area: parseFloat(areaInput.value),
        gap: parseFloat(gapInput.value),
        safetyResistor: parseFloat(safetyResistorInput.value),
        costPerSecond: parseFloat(costPerSecondInput.value),
        maxDuration: parseFloat(maxDurationInput.value),
        maxWaterPerSession: parseFloat(maxWaterInput.value)
    };

    try {
        localStorage.setItem('waterMeterSettings', JSON.stringify(settings));

        statusText.textContent = 'Settings saved!';
        statusText.style.color = '#10b981';
        console.log('✓ Settings saved to localStorage:', settings);

        setTimeout(() => {
            statusText.textContent = 'Ready';
            statusText.style.color = '';
        }, 2000);

        closeSettings();
    } catch (error) {
        console.error('Error saving settings:', error);
        statusText.textContent = 'Error saving settings';
        statusText.style.color = '#ef4444';
    }
}

// Show test indicator
const testBanner = document.createElement('div');
testBanner.style.cssText = `
    position: fixed;
    bottom: 0;
    left: 0;
    right: 0;
    background: #d97757;
    color: white;
    text-align: center;
    padding: 0.5rem;
    font-size: 0.875rem;
    font-weight: 500;
    z-index: 9999;
`;
testBanner.textContent = '🧪 TEST MODE - Using mock responses with real API pricing - Settings saved to browser localStorage';
document.body.appendChild(testBanner);
