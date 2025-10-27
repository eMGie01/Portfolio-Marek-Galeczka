class ESP32Control {
    constructor() {
        this.baseUrl = 'http://192.168.4.1';
        this.ledState = false;
        this.init();
    }

    init() {
        document.getElementById('led-btn').addEventListener('click', () => this.ledToggle());
        this.startModbusPolling();
    }

    startModbusPolling() {
        setInterval(() => this.updateModbusData(), 2000);
        this.updateModbusData();
    }

    updateModbusData() {
        fetch(`${this.baseUrl}/api/modbus`)
            .then(response => response.json())
            .then(data => {
                document.getElementById('mb_value').innerText = data.value;
                document.getElementById('mb_hex').innerText = data.hex;
                document.getElementById('mb_device').innerText = data.device_id;
                
                // Aktualizuj timestamp
                this.lastUpdate = new Date();
                this.updateTimestamp();
                
                this.highlightNewData();
            })
            .catch(error => {
                console.error('MODBUS Error:', error);
                document.getElementById('mb_timestamp').innerText = 'Błąd połączenia';
                document.getElementById('mb_timestamp').style.color = '#e74c3c';
            });
    }

    updateTimestamp() {
        if (this.lastUpdate) {
            const now = new Date();
            const timeString = this.lastUpdate.toLocaleTimeString('pl-PL', {
                hour: '2-digit',
                minute: '2-digit',
                second: '2-digit'
            });
            document.getElementById('mb_timestamp').innerText = timeString;
            document.getElementById('mb_timestamp').style.color = '#7f8c8d';
        }
    }

    highlightNewData() {
        const elements = document.querySelectorAll('#mb_value, #mb_hex, #mb_device');
        elements.forEach(el => {
            el.classList.add('data-updated');
            setTimeout(() => el.classList.remove('data-updated'), 1000);
        });
    }

    async ledToggle() {
        const button = document.getElementById('led-btn');
        const originalText = button.textContent;

        button.classList.add('loading');
        button.textContent = 'LED...';
        button.disabled = true;

        try {
            const response = await fetch(`${this.baseUrl}/api/toggle`, {
                method: 'POST'
            });

            if (response.ok) {
                const data = await response.json();
                this.ledState = data.led_state;
                
                button.textContent = this.ledState ? 'LED ON!' : 'LED OFF!';
                button.style.background = this.ledState ?
                    'linear-gradient(135deg, #27ae60, #229954)' :
                    'linear-gradient(135deg, #e74c3c, #c0392b)';
                    
            } else {
                throw new Error('Błąd serwera');
            }
        } catch (error) {
            console.error('Błąd:', error);
            button.textContent = 'Error!';
        } finally {
            setTimeout(() => {
                button.classList.remove('loading');
                button.textContent = 'Led Toggle';
                button.disabled = false;
            }, 2000);
        }
    }
}

document.addEventListener('DOMContentLoaded', () => {
    new ESP32Control();
});