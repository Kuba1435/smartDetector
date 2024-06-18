import React, {useState, useEffect} from 'react';


import Login from './Web/Login/Login';
import Home from './Web/Main/Home/Home';


import './App.css';


function App() {
  const [logged , setLogged] = useState(false);


  if (!logged) {
    return (
      <div className="App">
        <Login setLogged={setLogged} />
      </div>
    );
  }
  else {
      return (
        <div className="App">
          <Home />
        </div>
      );
  }
}

export default App;
