import React, { useState } from 'react';
import './login.css';

document.title = 'Přihlášení';


export default function Login({ setLogged }) {
  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");
  const [errorMessage, setErrorMessage] = useState(null);


  const handleSubmit = async (event) => {
    event.preventDefault();

    const response = await fetch('/api/login', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({ username, password })
    });

    if (response.status === 401) {
      setErrorMessage('Neplatné přihlašovací údaje');
    } else if (response.status === 200) {
      const data = await response.json();
      console.log(data);
      setLogged(true);
      setErrorMessage(null);
    }
  };

  return (
    <body className='loginBody'>
      <div id="login" className='loginWindow'>
        <h1 className='loginH1'>Přihlášení</h1>
        {errorMessage && <p className='errorMessage'>{errorMessage}</p>}
        <form onSubmit={handleSubmit} className='loginForm'>
          <input className="loginInput" type="text" name="username" placeholder="Uživatelské jméno" required onChange={e => setUsername(e.target.value)} />
          <input className="loginInput"  type="password" name="password" placeholder="Heslo" required onChange={e => setPassword(e.target.value)} />
          <button type="submit" className='loginButton'>Přihlásit se</button>
        </form>
      </div>
    </body>
  )
}