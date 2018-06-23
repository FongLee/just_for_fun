import React, { Component } from 'react';

import './App.css';

class App extends Component {
  render() {
    var helloWorld = 'welcome to the Road to learn React';
    var name = {
      firstName: 'li',
      lastName: 'feng'
    };
    return (
      <div className="App">
        <h2>{helloWorld}</h2>
        <p>name is {name.firstName} {name.lastName}</p>
      </div>
    );
  }
}

export default App;
