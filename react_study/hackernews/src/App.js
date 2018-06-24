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
class App extends Component {
  render() {

    return (
      <div className="App">
        {list.map(function(item) {
          return (
            <div key={itme.objectId}>
              <span>
                  <a href={item.url}>{item.title}</a>
              </span> 
              <span>{item.author}</span>
            </div>
          )
        })}
      </div>
    );
  }
}

export default App;
