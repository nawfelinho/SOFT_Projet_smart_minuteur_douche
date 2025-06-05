import React, { useEffect, useState } from 'react';

function App() {
  const [users, setUsers] = useState([]);

  useEffect(() => {
    fetch("http://localhost:8080/users")
        .then(response => response.json())
        .then(data => setUsers(data))
        .catch(error => console.error("Erreur lors du chargement des utilisateurs :", error));
  }, []);

  return (
      <div style={{ padding: '2rem' }}>
        <h1>Liste des utilisateurs</h1>
        {users.length === 0 ? (
            <p>Aucun utilisateur trouvé.</p>
        ) : (
            <ul>
              {users.map(user => (
                  <li key={user.id}>
                    {user.prenom} {user.nom} — Temps total : {user.tempsTotal} sec
                  </li>
              ))}
            </ul>
        )}
      </div>
  );
}

export default App;
