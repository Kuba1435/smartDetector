import React, { useState, useEffect } from 'react'
import CIcon from '@coreui/icons-react';
import * as icon from '@coreui/icons';

import './home.css';

import DeviceEdit from './DeviceEdit';

document.title = 'Home';

export default function Home() {
    const [currentTime, setCurrentTime] = useState(new Date());
    const [devices, setDevices] = useState([]);
    const [deviceData, setDeviceData] = useState([]);
    const [showDeviceInfo, setShowDeviceInfo] = useState(false);

    useEffect(() => {
        const timer = setInterval(() => {
            setCurrentTime(new Date());
        }, 1000);

        return () => {
            clearInterval(timer);
        };
    }, []);

    useEffect(() => {
        fetch('/api/devices')
            .then(response => response.json())
            .then(data => {setDevices(data); console.log(data)});
    }, []);


    const handleShowDeviceInfo = (id) => {
        fetch('/api/devices/' + id + '/info')
            .then(response => response.json())
            .then(data => {setDeviceData(data); console.log(data); setShowDeviceInfo(true)});
        }

    const reloadDevices = () => {
        fetch('/api/devices')
            .then(response => response.json())
            .then(data => {setDevices(data); console.log(data)});
    }



    return (
          <div className='body'>
              <header>
                <h1>Božotron</h1>
                <div className='time'>
                    {currentTime.toLocaleDateString()} {currentTime.toLocaleTimeString()}
                </div>
                <button>Přidat zařízení</button>
              </header>

              {!showDeviceInfo ?
               <div className='devices'>
                    {devices.map(device => (
                        <div className='device' key={device.id}>
                            <CIcon icon={icon.cilLoopCircular} className='icon' onClick={() => reloadDevices()}/>
                            <h2>{device[1]}</h2>
                            <div className='deviceStatus'>
                                <div className='statusBall' style={device[3] === "ONLINE" ? {backgroundColor:"green"} : {backgroundColor: "rgb(143, 13, 13)"}}></div>
                                <p>{device[2]}</p>
                            </div>
                            <button onClick={() => handleShowDeviceInfo(device[0])}>Podrobnosti</button>
                        </div>
                    ))}	
                </div> :


                <DeviceEdit device={deviceData} onClose={() => setShowDeviceInfo(false)} />}

            

          </div>
    )
}