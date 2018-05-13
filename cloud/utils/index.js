const { readFile, writeFile } = require('fs')
const { buildSchema } = require("graphql")


module.exports =  {
    prepSchema: () => readFile('./schema.gql', 'utf8', (e, Definition) => e
        ?   console.error(e)
        :   buildSchema(Definition) && //syntax check
            writeFile(
            './.serverless/schema.json',
            JSON.stringify({ Definition }),
            'utf8',
            e => console.error(e)
           )
    )
}


