import React, { Component } from 'react';

import './App.css';


const list = [
  {
    title: 'React',
    url: 'https://facebook.github.io/react/',
    author: 'Jordan Walke',
    num_comments: 3,
    points: 4,
    objectID: 0,
  },
  {
    title: 'Redux',
    url: 'https://github.com/reactjs/redux',
    author: 'Dan Abramov, Andrew Clark',
    num_comments: 2,
    points: 5,
    objectID: 1,
  },
];

const myList = {list};

const userService = {
  getUserName(user) {
    return user.firstName + ' ' + user.lastName;
  }
}

function  isSearched(searchTerm) {
    return function(item) {
      return item.title.toLowerCase().includes(searchTerm.toLowerCase());
    }
} 

class App extends Component {

  constructor(props) {
    super(props);
    this.state = {
      list: list,
      searchTerm: ''
    };
    this.onDismiss = this.onDismiss.bind(this);

    this.onSearchChange = this.onSearchChange.bind(this);
  } 

  onDismiss (id) {
    debugger;
    const updateList = this.state.list.filter(function(item) {
      return (item.objectID !== id)
    });

    this.setState({list: updateList});
  }

  onSearchChange (event) {
    debugger;

    this.setState({ searchTerm: event.target.value });
  }

  

  render() {
    return (
      <div className="App">

          <form>
            <input type="text" onChange={this.onSearchChange}/>
          </form>
          {this.state.list.filter(isSearched(this.state.searchTerm)).map(item => {
            const onHandleDismiss = () =>
              this.onDismiss(item.objectID);

            return (
              <div key={item.objectID}>
                <span>
                  <a href={item.url}>{item.title}</a>
                </span>
                <span>{item.author}</span>
                <span>{item.num_comments}</span>
                <span>{item.points}</span>
                <span>
                  <button
                    onClick={onHandleDismiss}
                    type="button"
                  >
                    Dismiss
                  </button>
                </span>
              </div>
            );
          }
        )}
      </div>
    );
  }
}

export default App;