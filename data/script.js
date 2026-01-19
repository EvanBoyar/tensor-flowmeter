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
const wifiSSIDInput = document.getElementById('wifiSSID');
const wifiPasswordInput = document.getElementById('wifiPassword');
const apiKeyInput = document.getElementById('apiKey');
const modelNameInput = document.getElementById('modelName');
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
let statusUpdateInterval = null;

// Initialize
document.addEventListener('DOMContentLoaded', () => {
    loadSettings();
    startStatusUpdates();
    setupEventListeners();
    autoResizeTextarea();
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

    const fetchModelsBtn = document.getElementById('fetchModelsBtn');
    if (fetchModelsBtn) {
        fetchModelsBtn.addEventListener('click', fetchAvailableModels);
    }
}

// Auto-resize textarea
function autoResizeTextarea() {
    messageInput.style.height = 'auto';
    messageInput.style.height = Math.min(messageInput.scrollHeight, 200) + 'px';
}

// Send message to Claude
async function sendMessage() {
    if (isSending) return;

    const message = messageInput.value.trim();
    if (!message) return;

    isSending = true;
    sendBtn.disabled = true;
    statusText.textContent = 'Sending...';

    // Add user message to chat
    addMessage('user', message);
    messageInput.value = '';
    autoResizeTextarea();

    try {
        const response = await fetch('/chat', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ message })
        });

        const data = await response.json();

        if (data.error) {
            statusText.textContent = 'Error: ' + data.error;
            addMessage('error', 'Error: ' + data.error);
        } else {
            // Add assistant message
            addMessage('assistant', data.response, {
                cost: data.cost,
                inputTokens: data.inputTokens,
                outputTokens: data.outputTokens
            });
            statusText.textContent = 'Ready';
        }
    } catch (error) {
        console.error('Error:', error);
        statusText.textContent = 'Network error';
        addMessage('error', 'Network error: ' + error.message);
    }

    isSending = false;
    sendBtn.disabled = false;
    messageInput.focus();
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
async function startNewSession() {
    if (!confirm('Start a new session? This will clear the current conversation and reset water usage.')) {
        return;
    }

    try {
        const response = await fetch('/new-session', {
            method: 'POST'
        });

        if (response.ok) {
            // Clear chat
            chatMessages.innerHTML = `
                <div class="welcome-message">
                    <h2>Welcome to AI Water User</h2>
                    <p>This project makes AI resource consumption tangible by destroying water through electrolysis. Each AI response triggers a reaction proportional to the computational cost, literally turning invisible electricity usage into visible water loss.</p>
                    <p><strong>To get started:</strong> Click the settings button (⚙️) and configure your Anthropic API key.</p>
                    <p>Then start a conversation below and watch the Tensor Flow Meter as water is consumed in real-time.</p>
                </div>
            `;

            // Reset water display
            waterValue.textContent = '0.000000 µg';
            statusText.textContent = 'New session started';
        }
    } catch (error) {
        console.error('Error starting new session:', error);
        statusText.textContent = 'Error starting new session';
    }
}

// Settings management
function openSettings() {
    settingsPanel.classList.remove('hidden');
}

function closeSettings() {
    settingsPanel.classList.add('hidden');
}

async function loadSettings() {
    try {
        const response = await fetch('/settings');
        const settings = await response.json();

        wifiSSIDInput.value = settings.wifiSSID || '';
        wifiPasswordInput.value = settings.wifiPassword || '';
        apiKeyInput.value = settings.apiKey || '';
        modelNameInput.value = settings.modelName || 'claude-3-haiku-20240307';
        voltageInput.value = settings.voltage;
        vThresholdInput.value = settings.vThreshold;
        conductivityInput.value = settings.conductivity;
        areaInput.value = settings.area;
        gapInput.value = settings.gap;
        safetyResistorInput.value = settings.safetyResistor;
        costPerSecondInput.value = settings.costPerSecond;
        maxDurationInput.value = settings.maxDuration;
        maxWaterInput.value = settings.maxWaterPerSession;
    } catch (error) {
        console.error('Error loading settings:', error);
    }
}

async function saveSettings() {
    const settings = {
        wifiSSID: wifiSSIDInput.value,
        wifiPassword: wifiPasswordInput.value,
        apiKey: apiKeyInput.value,
        modelName: modelNameInput.value,
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
        const response = await fetch('/settings', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(settings)
        });

        if (response.ok) {
            const data = await response.json();
            console.log('Settings saved successfully:', data);
            statusText.textContent = 'Settings saved';
            closeSettings();
        } else {
            const errorText = await response.text();
            console.error('Settings save failed:', response.status, errorText);
            statusText.textContent = `Error saving settings (${response.status})`;
        }
    } catch (error) {
        console.error('Error saving settings:', error);
        statusText.textContent = 'Error saving settings: ' + error.message;
    }
}

// Status updates (water usage)
function startStatusUpdates() {
    statusUpdateInterval = setInterval(updateStatus, 500); // Update every 500ms
}

async function updateStatus() {
    try {
        const response = await fetch('/status');
        const status = await response.json();

        // Update water display with animation (convert to micrograms)
        const currentText = waterValue.textContent.replace(' µg', '');
        const currentMicrograms = parseFloat(currentText);
        const newWaterGrams = status.totalWaterUsed;
        const newMicrograms = newWaterGrams * 1000000;

        if (Math.abs(newMicrograms - currentMicrograms) > 0.001) {
            waterValue.textContent = newMicrograms.toFixed(6) + ' µg';

            // Add pulsing effect if electrolysis is active
            if (status.isElectrolysisActive) {
                waterValue.style.animation = 'none';
                setTimeout(() => {
                    waterValue.style.animation = 'pulse 1s ease-in-out infinite';
                }, 10);
            } else {
                waterValue.style.animation = '';
            }
        }
    } catch (error) {
        // Silently fail - don't spam console with network errors
    }
}

// Fetch available models from Anthropic API
async function fetchAvailableModels() {
    const modelsList = document.getElementById('modelsList');
    const fetchBtn = document.getElementById('fetchModelsBtn');

    fetchBtn.disabled = true;
    fetchBtn.textContent = 'Fetching...';
    modelsList.innerHTML = 'Loading available models...';
    modelsList.style.display = 'block';

    try {
        const response = await fetch('/models');
        const data = await response.json();

        if (data.error) {
            modelsList.innerHTML = `<span style="color: #e74c3c;">Error: ${data.error}</span>`;
        } else if (data.data && Array.isArray(data.data)) {
            // Build list of models
            let html = '<div style="margin-top: 8px;"><strong>Available Models:</strong></div>';
            html += '<ul style="margin: 4px 0; padding-left: 20px; list-style: none;">';

            data.data.forEach(model => {
                html += `<li style="margin: 4px 0;">
                    <a href="#" class="model-link" data-model="${model.id}" style="color: #3498db; text-decoration: none; cursor: pointer;">
                        ${model.id}
                    </a>
                    ${model.display_name ? ` - ${model.display_name}` : ''}
                </li>`;
            });

            html += '</ul>';
            modelsList.innerHTML = html;

            // Add click handlers to model links
            const modelLinks = modelsList.querySelectorAll('.model-link');
            modelLinks.forEach(link => {
                link.addEventListener('click', (e) => {
                    e.preventDefault();
                    const modelId = e.target.getAttribute('data-model');
                    modelNameInput.value = modelId;
                    modelsList.innerHTML = `<span style="color: #27ae60;">Selected: ${modelId}</span>`;
                });
            });
        } else {
            modelsList.innerHTML = '<span style="color: #e74c3c;">No models found in response</span>';
        }
    } catch (error) {
        console.error('Error fetching models:', error);
        modelsList.innerHTML = `<span style="color: #e74c3c;">Network error: ${error.message}</span>`;
    }

    fetchBtn.disabled = false;
    fetchBtn.textContent = 'Fetch Available Models';
}

// Add pulse animation
const style = document.createElement('style');
style.textContent = `
    @keyframes pulse {
        0%, 100% { opacity: 1; }
        50% { opacity: 0.7; }
    }
`;
document.head.appendChild(style);
