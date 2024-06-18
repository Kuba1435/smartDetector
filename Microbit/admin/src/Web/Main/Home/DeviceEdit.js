import React, { useEffect, useState } from 'react'

import "./deviceEdit.css";

export default function DeviceEdit({onClose, device}) {
    document.title = 'Informace o zařízení';
    const [notices, setNotices] = useState([]);
    const [deviceID, setDeviceID] = useState(device[0]);

    const [deviceName, setDeviceName] = useState(device[1]);
    const [deviceSpeed, setDeviceSpeed] = useState(device[6]);
    const [deviceSoundAlarm, setDeviceSoundAlarm] = useState(device[3]);
    const [deviceLightAlarm, setDeviceLightAlarm] = useState(device[4]);


    useEffect(() => {
        

        fetch('/api/devices/' + device[0] + '/notices')
            .then(response => response.json())
            .then(data => setNotices(data));
    }, [deviceID]);


    const handleSubmit = (event) => {
        fetch('/api/devices/' + deviceID + '/edit', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({name: deviceName, speed: deviceSpeed, soundAlarm: deviceSoundAlarm, lightAlarm: deviceLightAlarm}),
        })
        .then(response => response.json())
        .then(data => console.log(data));
    }


    
    return (
        <div>
            <button onClick={onClose} className='deviceEditCloseButtn'>Zavřít</button>
            <div className='deviceEdit'>

                <div className='deviceNotice'>
                    <h2>Upozornění</h2>
                    <div className='noticeList'>
                        {notices.map(notice => (
                            <div className='notice'>
                                <div className='ball'></div>
                                <div className='noticeTime'>{notice[3]}</div>
                                <div className='noticeText'>{notice[2]}</div>
                            </div>
                        ))}
                    </div>

                    
                </div>

                <div className='deviceInfo'>
                    <h2>Informace o zařízení</h2>

                    <form onSubmit={() => handleSubmit()}>
                        <label>
                            <span>Název zařízení:</span>
                            <input type='text' value={deviceName} defaultValue={deviceName}  onChange={(state) => setDeviceName(state.target.value)} />
                        </label>
                        <label>
                            <span>Rychlost snímání [s]:</span>
                            <input type='number' value={deviceSpeed} defaultValue={deviceSpeed} onChange={(state) => setDeviceSpeed(state.target.value)}/>
                        </label>
                        <label>
                            <span>Zvukový alarm:</span>
                            <input type="checkbox" checked={!!deviceSoundAlarm} onChange={() => setDeviceSoundAlarm(!deviceSoundAlarm) } />
                        </label>
                        <label>
                            <span>Světelný alarm:</span>
                            <input type='checkbox' checked={!!deviceLightAlarm} onChange={() => setDeviceLightAlarm(!deviceLightAlarm) }/>
                        </label>

                        <button type='submit'>Uložit</button>
                    </form>
                </div>
            </div>
        </div>
    )
  
}

