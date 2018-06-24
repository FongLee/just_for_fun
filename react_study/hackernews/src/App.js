import React, { Component } from 'react';

import './App.css';

const list = [
{
  title: 'react',
  author: 'lifeng',
  url: 'www.baidu.com',
  objectId: 0
},
{
  title: 'java',
  author: 'li',  
  url: 'www.google.com',
  objectId: 1
}];

const myList = {list};

const userService = {
  getUserName(user) {
    return user.firstName + ' ' + user.lastName;
  }
}

class App extends Component {

  constructor(props) {
    super(props);
    this.state = {
      list: list
    };
  }

  render() {

    return (
      <div className="App">
        {this.state.list.map(item =>
          <div key={item.objectID}>
            <span>
              <a href={item.url}>{item.title}</a>
            </span>
            <span>{item.author}</span>
          </div>
        )}
        
      </div>
    );
  }
}

export default App;
